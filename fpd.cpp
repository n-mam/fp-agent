#include <map>
#include <iostream>

#include <OSManager.hpp>
#include <FTPManager.hpp>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "usage : Agent <host> <port>\n";
    std::cout << "usage : Agent 0.0.0.0 8081\n";
    return 0;
  }

  auto host = std::string(argv[1]);

  auto port = std::stoi(argv[2]);

  CManager::AddManagerToMMap("os", std::make_shared<COSManager>());
  CManager::AddManagerToMMap("ftp", std::make_shared<CFTPManager>());

  /*
   * start the websocket server
   */
  auto ws = NPL::make_ws_server(
    host, port, TLS::YES,
    [] (SPCProtocol c, const std::string& message) 
    {
      std::cout << "client : " << message << "\n";

      Json json(message);

      auto key = json.GetKey("app");

      if (key.length())
      {
        ManagerMap[key]->Dispatch(c, json);
      }
      else
      {
        c->SendProtocolMessage(
          (uint8_t *)"Error : unknown application request", 
          strlen("Error : unknown application request")
        );        
      }
    }
  );

  ws->StartServer();

  getchar();

  return 0;
}
