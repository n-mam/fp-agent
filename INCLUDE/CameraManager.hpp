#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <cvl.hpp>
#include <Manager.hpp>
#include <Encryption.hpp>

class CCameraManager : public CManager
{
  public:

    void Dispatch(NPL::SPCProtocol c, Json& json) override
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
          else if (action == "set-property")
          {
            CameraSetProperty(json);
          }
          else if (action == "get-property")
          {
            CameraGetProperty(json);
          }
          else
          {
            json.SetKey("error", "Unknown action : " + action);
          }
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
      auto algo = json.GetKey("algo");
      auto tracker = json.GetKey("tracker");

      if (!sid.size() ||
          !source.size() ||
          !target.size() ||
          !tracker.size())
      {
        json.SetKey("error", "create : invalid input");
        return;
      }

      #ifdef OPENVINO
        auto camera = CVL::make_camera(source, target);
      #else
        auto camera = CVL::make_camera(source, target, algo, tracker);
      #endif

      auto bbarea = json.GetKey("bbarea");
      if (bbarea.size()) camera->SetProperty("bbarea", bbarea);
      auto exhzbb = json.GetKey("exhzbb");
      if (exhzbb.size()) camera->SetProperty("exhzbb", exhzbb);
      auto skipcount = json.GetKey("skipcount");
      if (skipcount.size()) camera->SetProperty("skipcount", skipcount);

      SessionMap.insert(std::make_pair(sid, camera));

      camera->OnConnect();
    }

    void DeleteCameraSession(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      camera->OnDisconnect();

      auto sid = json.GetKey("sid");

      if (camera->IsStarted())
      {
        camera->Stop();
      }

      SessionMap.erase(sid);
    }

    void CameraStart(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      auto sid = json.GetKey("sid");
      auto cid = json.GetKey("cid");
      auto aid = json.GetKey("aid");
      auto uid = json.GetKey("uid");
      auto aep = json.GetKey("aep");

      camera->Start(
        [this, cid, sid, aid, uid, aep](const std::string& e, const std::string& data1, const std::string& data2, std::vector<uint8_t>& frame)
        {
          if (e == "stop")
          {
            CameraStopEvent(sid);
          }
          else if (e == "trail")
          {
            CameraTrailEvent(cid, aid, uid, data1, data2, aep);
          }
          else if (e == "play")
          {
            CameraPlayEvent(sid, frame);
          }
        });
    }

    void CameraStop(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      auto sid = json.GetKey("sid");

      camera->Stop();
    }

    void CameraPlay(Json& json)
    {
      auto sid = json.GetKey("sid");

      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      if (!camera->IsStarted())
      {
        json.SetKey("error", "camera session not started");
        return;
      }

      camera->Play();
    }

    void CameraStopPlay(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      camera->StopPlay();
    }

    void CameraPause(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      if (camera->IsStarted())
      {
        camera->Pause();
      }
    }

    void CameraForward(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      camera->Forward();
    }

    void CameraBackward(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      camera->Backward();
    }

    void CameraSetProperty(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      auto prop = json.GetKey("prop");
      auto value = json.GetKey("value");

      camera->SetProperty(prop, value);
    }

    void CameraGetProperty(Json& json)
    {
      auto camera = GetTargetCamera(json);

      if (!camera)
      {
        json.SetKey("error", "camera session not found");
        return;
      }

      auto prop = json.GetKey("prop");

      json.SetKey(prop, camera->GetProperty(prop));
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
      const std::string& points,
      const std::string& demography,
      const std::string& aep)
    {
      auto http = NPL::make_http_client(
        (aep == "localhost") ?
           "127.0.0.1" : "52.66.251.154",
        8080);

      http->StartClient(
       [cid, aid, uid, points, demography](auto p)
       {
         Json j;
         j.SetKey("api", "TRAIL");
         j.SetKey("cid", cid);
         j.SetKey("aid", aid);
         j.SetKey("uid", uid);
         j.SetKey("points", points);
         j.SetKey("demography", demography);

         auto c = std::dynamic_pointer_cast<NPL::CProtocolHTTP>(p);

         if (c)
         {
           c->Post("/api", j.Stringify());
         }
       }
      );
    }

    void CameraPlayEvent(const std::string& sid, std::vector<uint8_t>& frame)
    {
      Json j;
      j.SetKey("app", "cam");
      j.SetKey("sid", sid);
      j.SetKey("req", "play");
      char encoded[360*500];
      int n = Base64Encode((unsigned char *)encoded, frame.data(), frame.size());
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