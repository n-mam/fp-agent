#include <iostream>
#include <npl.hpp>

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

  auto ws = NPL::make_ws_server(
    host, port, TLS::YES, 
    [] (SPCProtocol c, const std::string& m) 
    {
      std::cout << "client : " << m << "\n";

      c->SendProtocolMessage(
        (uint8_t *)"server echo : hello", 
        strlen("server echo : hello")
      );
    }
  );

  ws->StartServer();

  getchar();

  return 0;
}