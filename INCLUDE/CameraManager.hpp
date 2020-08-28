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
        else if (req == "camera-control")
        {
          auto action = json.GetKey("action");

          if (action == "create")
          {
            CreateCameraSession(json);
          }
          else if (action == "delete")
          {
            DeleteCameraSession(json);
          }
          else if (action == "start")
          {
            CameraStart(json);
          }
          else if (action == "stop")
          {
            CameraStop(json);
          }
          else if (action == "play")
          {
            CameraPlay(json);
          }
          else if (action == "pause")
          {
            CameraPause(json);
          }
          else if (action == "stop-play")
          {
            CameraStopPlay(json);
          }
          else if (action == "forward")
          {
            CameraForward(json);
          }
          else if (action == "backward")
          {
            CameraBackward(json);
          }
        }
      }
      else
      {

      }
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

      if (!sid.size() ||
          !source.size() ||
          !target.size() ||
          !tracker.size())
      {
        SendErrorResponse("create : invalid input");
        return;
      }

      auto camera = CVL::make_camera(source, target, tracker);

      //camera->SetName(sid);

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

      if (camera->IsStarted())
      {
        camera->Stop();
      }

      SessionMap.erase(sid);

      SendResponse(json);
    }

    void CameraStart(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      auto sid = json.GetKey("sid");
      auto cid = json.GetKey("cid");
      auto aid = json.GetKey("aid");
      auto uid = json.GetKey("uid");

      camera->Start(
        [this, cid, sid, aid, uid](const std::string& e, const std::string& data)
        {
          if (e == "stop")
          {
            CameraStopEvent(sid);
          }
          else if (e == "trail")
          {
            CameraTrailEvent(cid, aid, uid, data);
          }
          else if (e == "play")
          {
            CameraPlayEvent(sid, data);
          }
        });

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

      camera->Stop();

      SendResponse(json);
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

      if (!camera->IsStarted())
      {
        SendErrorResponse("camera session not started");
        return;
      }

      camera->Play();

      SendResponse(json);
    }

    void CameraStopPlay(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      camera->StopPlay();

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

    void CameraForward(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      camera->Forward();

      SendResponse(json);
    }

    void CameraBackward(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        SendErrorResponse("camera session not found");
        return;
      }

      camera->Backward();

      SendResponse(json);
    }

  protected:

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

    void CameraStopEvent(const std::string& sid)
    {
      Json j;
      j.SetKey("app", "cam");
      j.SetKey("sid", sid);
      j.SetKey("req", "camera-stop");
      SendResponse(j);
    }
  
    void CameraTrailEvent(
      const std::string& cid,
      const std::string& aid,
      const std::string& uid,
      const std::string& points)
    {
      auto http = NPL::make_http_client("127.0.0.1", 8080);

      http->StartClient(
       [cid, aid, uid, points](auto p)
       {
         Json j;
         j.SetKey("api", "TRAIL");
         j.SetKey("cid", cid);
         j.SetKey("aid", aid);
         j.SetKey("uid", uid);
         j.SetKey("points", points);

         auto c = std::dynamic_pointer_cast<NPL::CProtocolHTTP>(p);

         if (c)
         {
           c->Post("/api", j.Stringify());
         }
       }
      );
    }

    void CameraPlayEvent(const std::string& sid, const std::string& encoded)
    {
      Json j;
      j.SetKey("app", "cam");
      j.SetKey("sid", sid);
      j.SetKey("req", "play");
      j.SetKey("frame", encoded);
      SendResponse(j);
    }

    void SendErrorResponse(const std::string& e)
    {
      Json j;
      j.SetKey("error", e);
      SendResponse(j);
    }

};

using SPCCameraManager = std::shared_ptr<CCameraManager>;
using WPCCameraManager = std::weak_ptr<CCameraManager>;

#endif