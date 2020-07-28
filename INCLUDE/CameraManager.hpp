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

        if (req == "camera-create")
        {
          CreateCameraSession(json);
        }
        else if (req == "camera-delete")
        {
          DeleteCameraSession(json);
        }
        else if (req == "camera-control")
        {
          auto action = json.GetKey("action");

          if (action == "play")
          {
            CameraPlay(json);
          }
          else if (action == "pause")
          {
            CameraPause(json);
          }
          else if (action == "stop")
          {
            CameraStop(json);
          }
          else if (action == "forward")
          {

          }
          else if (action == "backward")
          {

          }
        }
      }
      else
      {

      }
    }

    void CreateCameraSession(Json& json)
    {
      auto sid = json.GetKey("sid");
      auto source = json.GetKey("source");
      auto target = json.GetKey("target");
      auto tracker = json.GetKey("tracker");

      auto camera = CVL::make_camera(source, target, tracker);

      camera->SetName(sid);

      SessionMap.insert(std::make_pair(sid, camera));

      camera->OnConnect();

      SendResponse(json);
    }

    void DeleteCameraSession(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>(SessionMap[sid]);

      camera->OnDisconnect();

      camera->Stop([this, sid](){
        CameraStopCallback(sid);
      });
    }

    void CameraPlay(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>(SessionMap[sid]);

      if (camera->IsStarted() && !camera->IsPaused())
      {
        return;
      }

      if (!camera->IsStarted())
      {
        camera->Start([this, sid](){
          CameraStopCallback(sid);
        });
      }

      camera->Play([this, sid](const std::string& encoded){
        Json j;
        j.SetKey("app", "cam");
        j.SetKey("sid", sid);
        j.SetKey("req", "play");
        j.SetKey("frame", encoded);
        SendResponse(j);
      });
    }

    void CameraPause(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>(SessionMap[sid]);

      if (camera->IsStarted())
      {
        camera->Pause();
      }

      SendResponse(json);
    }

    void CameraStop(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = std::dynamic_pointer_cast<CCamera>(SessionMap[sid]);

      camera->Stop([this, sid](){
        CameraStopCallback(sid);
      });
    }

    void CameraStopCallback(const std::string& sid)
    {
      Json j;
      j.SetKey("app", "cam");
      j.SetKey("sid", sid);
      j.SetKey("req", "camera-stop");
      SendResponse(j);
    }
};

using SPCCameraManager = std::shared_ptr<CCameraManager>;
using WPCCameraManager = std::weak_ptr<CCameraManager>;

#endif