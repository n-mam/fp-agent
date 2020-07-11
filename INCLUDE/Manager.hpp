#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <memory>
#include <vector>

#include <npl.hpp>

class CManager;

using SPCManager = std::shared_ptr<CManager>;  

std::map<std::string, SPCManager> ManagerMap;
std::map<std::string, SPCProtocol> SessionMap;

class CManager
{ 
  public:

    CManager() {}

    ~CManager() {}

    virtual void Dispatch(SPCProtocol c, Json& req) = 0;

    void SendResponse(SPCProtocol c, Json& res)
    {
      auto message = res.Stringify();

      c->SendProtocolMessage(
          (uint8_t *)message.data(), 
          message.size()
        );   
    }

    static void AddManagerToMMap(const std::string& key, SPCManager value)
    {
      ManagerMap.insert(
        std::make_pair(key, value)
      );
    }    

  protected:

};

using WPCManager = std::weak_ptr<CManager>;


#endif //MANAGER_HPP