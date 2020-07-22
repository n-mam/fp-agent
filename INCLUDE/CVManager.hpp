#ifndef CVMANAGER_HPP
#define CVMANAGER_HPP

#include <cvl.hpp>
#include <Manager.hpp>

class CCVManager : public CManager
{
  public:

    void Dispatch(SPCProtocol c, Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "start-source")
        {
          StartSource(c, json);
        }
        else if (req == "stop-source")
        {
          StopSource(c, json);
        }
      }
      else
      {

      }
    }

    void StartSource(SPCProtocol c, Json& json)
    {

    }

    void StopSource(SPCProtocol c, Json& json)
    {

    }

};

using SPCCVManager = std::shared_ptr<CCVManager>;
using WPCCVManager = std::weak_ptr<CCVManager>;

#endif