#include <map>
#include <iostream>

#include <FTP.hpp>

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

  std::map<std::string, SPCManager> mm;

  mm.insert(
    std::make_pair(
      "ftp",
      std::make_shared<CFTPManager>()
    )
  );

  /*
   * start the websocket server
   */
  auto ws = NPL::make_ws_server(
    host, port, TLS::YES,
    [&mm] (SPCProtocol c, const std::string& message) 
    {
      std::cout << "client : " << message << "\n";

      Json json(message);

      auto app = json.GetKey("app");

      if (app.length())
      {
        mm[app]->Dispatch(c, json);
      }
      else
      {
        c->SendProtocolMessage(
          (uint8_t *)"Error : unknown application", 
          strlen("Error : unknown application")
        );        
      }
    }
  );

  ws->StartServer();

  getchar();

  return 0;
}
