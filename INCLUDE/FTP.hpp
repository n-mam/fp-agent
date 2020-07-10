#ifndef FTPMANAGER_HPP
#define FTPMANAGER_HPP

#include <memory>
#include <assert.h>

#include <Manager.hpp>

class CFTPManager : public CManager
{
  public:

    void Dispatch(SPCProtocol c, Json& json) override
    {
      if (json.HasKey("req"))
      {
        auto req = json.GetKey("req");

        if (req == "connect")
        {
          ConnectFTPSession(c, json);
        }
        else if (req == "list")
        {
          GetDirectoryList(c, json);
        }
      }
      else
      {
        
      }
    }

    void ConnectFTPSession(SPCProtocol c, Json& json)
    {
      auto host = json.GetKey("host");
      auto port = json.GetKey("port");
      auto user = json.GetKey("user");
      auto pass = json.GetKey("pass");
      auto ftps = json.GetKey("ftps");

      auto ftp = NPL::make_ftp(host, std::stoi(port), NPL::TLS::NO);

      ftp->SetCredentials(user, pass);

      ftp->StartClient();

      ftp->SetID(iSessions.size());

      iSessions.push_back(ftp);

      json.SetKey("id", std::to_string(ftp->GetID()));

      SendResponse(c, json);
    }

    void GetDirectoryList(SPCProtocol c, Json& json)
    {
      auto id = json.GetKey("id");
      auto dir = json.GetKey("dir");

      auto ftp = std::dynamic_pointer_cast<CProtocolFTP>
                     (iSessions[std::stoi(id)]);

      ftp->ListDirectory(
        [this, c, json, data = std::string("")] 
        (const char *b, size_t n) mutable {
          if (b) 
          {
            data.append(std::string(b, n));
          }
          else
          {
            json.SetKey("data", data);
            SendResponse(c, json);
          }
          return true;
        }, dir, NPL::DCProt::Protected);
    }

};

using SPCFTPManager = std::shared_ptr<CFTPManager>;
using WPCFTPManager = std::weak_ptr<CFTPManager>;

#endif //FTPMANAGER_HPP