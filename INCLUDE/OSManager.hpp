#ifndef OSMANAGER_HPP
#define OSMANAGER_HPP

#include <memory>
#include <assert.h>

#include <Manager.hpp>

class COSManager : public CManager
{
  public:

    void Dispatch(SPCProtocol c, Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "get-active-sessions")
        {
          GetActiveSessions(c, json);
        }
        else if (req == "get-volumes")
        {
          GetVolumes(c, json);
        }
      }
      else
      {

      }
    }

    void GetActiveSessions(SPCProtocol c, Json& json)
    {
      std::vector<Json> sv;

      for (auto& e : SessionMap)
      {
        Json s;
        
        s.SetKey("sid", e.first);
        
        sv.push_back(s);
      }

      auto sa = Json::JsonListToArray(sv);

      json.SetKey("sessions", sa);

      SendResponse(c, json);
    }

    void GetVolumes(SPCProtocol c, Json& json)
    {
      auto sid = json.GetKey("sid");


    }

};

using SPCOSManager = std::shared_ptr<COSManager>;
using WPCOSManager = std::weak_ptr<COSManager>;

#endif //OSMANAGER_HPP