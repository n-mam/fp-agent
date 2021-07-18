#include <map>
#include <iostream>

#include <OSManager.hpp>
#include <FTPManager.hpp>
#include <CameraManager.hpp>

int main(int argc, char *argv[])
{
  std::string host = "0.0.0.0";
  int port = 8081;

  if (argc != 3)
  {
    std::cout << "usage : Agent <host> <port>\n";
    std::cout << "using : Agent 0.0.0.0 8081\n";
  }
  else
  {
    host = std::string(argv[1]);
    port = std::stoi(argv[2]);
  }

  CManager::AddManagerToMMap("os", std::make_shared<COSManager>());
  CManager::AddManagerToMMap("cam", std::make_shared<CCameraManager>());
  CManager::AddManagerToMMap("ftp", std::make_shared<CFTPManager>());    

  /*
   * start websocket server
   */
  auto ws = NPL::make_ws_server(
    host, port, NPL::TLS::Yes,
    [] (NPL::SPCProtocol c, const std::string& message) 
    {
      std::cout << "client : " << message << "\n";

      Json json(message);

      auto key = json.GetKey("app");

      if (key.length())
      {
        auto& manager = ManagerMap[key];
        manager->AddClient(c);
        manager->Dispatch(c, json);
      }
      else
      {
        std::string m = "Error : unknown application request";
        c->SendProtocolMessage((uint8_t *) m.data(), m.size());        
      }
    }
  );

  ws->StartServer();

  getchar();

  return 0;
}
