#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <Manager.hpp>
#include <cvl.hpp>

class CCameraManager : public CManager
{
  public:

    void Dispatch(SPCProtocol c, Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "start-camera")
        {
          StartCameraSession(c, json);
        }
        else if (req == "stop-camera")
        {
          StopCameraSession(c, json);
        }
      }
      else
      {

      }
    }

    void StartCameraSession(SPCProtocol c, Json& json)
    {
      auto sid = json.GetKey("sid");
      auto rtsp = json.GetKey("url");

      auto camera = CVL::make_camera(rtsp);

      camera->Start();

      SessionMap.insert(std::make_pair(sid, camera));

      SendResponse(c, json);
    }

    void StopCameraSession(SPCProtocol c, Json& json)
    {

    }
};

using SPCCameraManager = std::shared_ptr<CCameraManager>;
using WPCCameraManager = std::weak_ptr<CCameraManager>;

#endif