#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <Manager.hpp>
#include <cvl.hpp>

class CCameraManager : public CManager
{
  public:

    void Dispatch(Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "start-camera")
        {
          StartCameraSession(json);
        }
        else if (req == "stop-camera")
        {
          StopCameraSession(json);
        }
        else if (req == "cam-play")
        {
          CameraPlay(json);
        }
        else if (req == "cam-pause")
        {
          CameraPause(json);
        }
        else
        {

        }
      }
      else
      {

      }
    }

    void StartCameraSession(Json& json)
    {
      auto sid = json.GetKey("sid");
      auto source = json.GetKey("source");
      auto target = json.GetKey("target");
      auto tracker = json.GetKey("tracker");

      auto camera = CVL::make_camera(source, target, tracker);

      camera->SetName(sid);

      camera->Start([this, sid](){
        CameraStoppedCallback(sid);
      });

      camera->OnConnect();

      SessionMap.insert(std::make_pair(sid, camera));

      SendResponse(json);
    }

    void StopCameraSession(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>
        (SessionMap[sid]);

      camera->Stop([this, sid](){
        CameraStoppedCallback(sid);
      });
    }

    void CameraStoppedCallback(const std::string& sid)
    {
      Json j;
      j.SetKey("app", "cam");
      j.SetKey("sid", sid);
      j.SetKey("req", "stop-camera");

      auto camera = std::dynamic_pointer_cast<CCamera>(SessionMap[sid]);

      camera->OnDisconnect();

      SendResponse(j);
    }

    void CameraPlay(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>
        (SessionMap[sid]);

      camera->Play([this, sid](const std::string& frame){
        Json j;
        j.SetKey("app", "cam");
        j.SetKey("sid", sid);
        j.SetKey("req", "stream");
        j.SetKey("frame", frame);
        SendResponse(j);
      });
    }

    void CameraPause(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>
        (SessionMap[sid]);

      camera->Pause();
    }
};

using SPCCameraManager = std::shared_ptr<CCameraManager>;
using WPCCameraManager = std::weak_ptr<CCameraManager>;

#endif