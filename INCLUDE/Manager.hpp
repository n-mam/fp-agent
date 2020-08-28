#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <memory>
#include <vector>
#include <assert.h>

#include <npl.hpp>

class CManager;

using SPCManager = std::shared_ptr<CManager>;  

std::map<std::string, SPCManager> ManagerMap;
std::map<std::string, NPL::SPCSubject<uint8_t, uint8_t>> SessionMap;

class CManager
{
  public:

    CManager() {}

    ~CManager() {}

    virtual void Dispatch(Json& req) = 0;

    virtual std::vector<Json> GetActiveSessions(Json& json)
    {
      std::vector<Json> sv;

      auto& it = SessionMap.begin();

      while (it != SessionMap.end())
      {
        if (((*it).second)->IsConnected())
        {
          Json s;
          s.SetKey("sid", (*it).first);
          sv.push_back(s);
          ++it;
        } 
        else
        {
          it = SessionMap.erase(it);
        }
      }

      return sv;
    }

    void SendResponse(Json& res)
    {
      auto r = res.Stringify();
      iClient->SendProtocolMessage((uint8_t *)r.data(), r.size());   
    }

    static void AddManagerToMMap(const std::string& key, SPCManager value)
    {
      ManagerMap.insert(std::make_pair(key, value));
    }

    static NPL::SPCProtocol iClient;
  
  protected:

};

NPL::SPCProtocol CManager::iClient = nullptr;

using WPCManager = std::weak_ptr<CManager>;

#endif //MANAGER_HPP