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

        if (req == "get-active-sessions")
        {
          GetActiveSessions(json);
        }
        else if (req == "camera-create")
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

    SPCCamera GetTargetCamera(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto it = SessionMap.find(sid);

      if (it != SessionMap.end())
      {
        return std::dynamic_pointer_cast<CCamera>(it->second);
      }

      return nullptr;
    }

    void SendErrorResponse(const std::string& e)
    {
      Json ej;
      ej.SetKey("error", e);
      SendResponse(ej);
    }

    virtual std::vector<Json> GetActiveSessions(Json& json) override
    {
      auto sl = CManager::GetActiveSessions(json);

      for (auto& s : sl)
      {
        auto camera = GetTargetCamera(s);

        if (!camera) continue;

        bool paused = camera->IsPaused();
        bool started = camera->IsStarted();

        s.SetKey("paused", paused ? "true" : "false");
        s.SetKey("started", started ? "true" : "false");
      }

      json.SetKey("sessions", Json::JsonListToArray(sl));

      SendResponse(json);

      return sl;
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
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      camera->OnDisconnect();

      auto sid = json.GetKey("sid");

      camera->Stop([this, sid](){
        CameraStopCallback(sid);
      });
    }

    void CameraPlay(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

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

      SendResponse(json);
    }

    void CameraPause(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      if (camera->IsStarted())
      {
        camera->Pause();
      }

      SendResponse(json);
    }

    void CameraStop(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      auto sid = json.GetKey("sid");

      camera->Stop([this, sid](){
        CameraStopCallback(sid);
      });

      SendResponse(json);
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