#ifndef OSMANAGER_HPP
#define OSMANAGER_HPP

#include <Manager.hpp>

class COSManager : public CManager
{
  public:

    void Dispatch(Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "get-volumes")
        {
          GetVolumes(json);
        }
      }
      else
      {

      }
    }

    void GetVolumes(Json& json)
    {

    }

};

using SPCOSManager = std::shared_ptr<COSManager>;
using WPCOSManager = std::weak_ptr<COSManager>;

#endif //OSMANAGER_HPP