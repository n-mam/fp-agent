#ifndef FTPMANAGER_HPP
#define FTPMANAGER_HPP

#include <Manager.hpp>

class CFTPManager : public CManager
{
  public:

    void Dispatch(Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "connect")
        {
          ConnectFTPSession(json);
        }
        else if (req == "list")
        {
          GetDirectoryList(json);
        }
      }
      else
      {
        
      }
    }

    void ConnectFTPSession(Json& json)
    {
      auto sid = json.GetKey("sid");
      auto host = json.GetKey("host");
      auto port = json.GetKey("port");
      auto user = json.GetKey("user");
      auto pass = json.GetKey("pass");
      auto ftps = json.GetKey("ftps");

      auto ftp = NPL::make_ftp(host, std::stoi(port), NPL::TLS::NO);

      ftp->SetCredentials(user, pass);

      ftp->StartClient();

      SessionMap.insert(std::make_pair(sid, ftp));

      SendResponse(json);
    }

    void GetDirectoryList(Json& json)
    {
      auto sid = json.GetKey("sid");
      auto dir = json.GetKey("dir");

      auto ftp = std::dynamic_pointer_cast<NPL::CProtocolFTP>
                     (SessionMap[sid]);

      ftp->ListDirectory(
        [this, json, data = std::string("")] 
        (const char *b, size_t n) mutable {
          if (b) 
          {
            data.append(std::string(b, n));
          }
          else
          {
            json.SetKey("data", data);
            SendResponse(json);
          }
          return true;
        }, dir, NPL::DCProt::Protected);
    }

};

using SPCFTPManager = std::shared_ptr<CFTPManager>;
using WPCFTPManager = std::weak_ptr<CFTPManager>;

#endif //FTPMANAGER_HPP