#ifndef OSMANAGER_HPP
#define OSMANAGER_HPP

#include <osl.hpp>
#include <Manager.hpp>

class COSManager : public CManager
{
  public:

    void Dispatch(NPL::SPCProtocol c, Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "get-volumes")
        {
          OSL::GetVolumes(json);
        }
        else
        {
          json.SetKey("error", "Unknown request : " + req);
        }
      }
      else
      {
        json.SetKey("error", "Request missing");
      }

      SendResponse(json);
    }

};

using SPCOSManager = std::shared_ptr<COSManager>;
using WPCOSManager = std::weak_ptr<COSManager>;

#endif //OSMANAGER_HPP