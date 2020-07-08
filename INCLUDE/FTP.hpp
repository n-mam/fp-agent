#ifndef FTPMANAGER_HPP
#define FTPMANAGER_HPP

#include <memory>
#include <assert.h>

#include <Manager.hpp>

class CFTPManager : public CManager
{
  public:

    void Dispatch(SPCProtocol c, Json& req) override
    {
      if (req.HasKey("cmd"))
      {
        auto cmd = req.GetKey("cmd");

        if (cmd == "connect")
        {
          CreateFTPSession(c, req);
        }
      }
      else
      {
        
      }
    }

    void CreateFTPSession(SPCProtocol c, Json& req)
    {
      auto host = req.GetKey("host");
      auto port = req.GetKey("port");
      auto user = req.GetKey("user");
      auto pass = req.GetKey("pass");
      auto ftps = req.GetKey("ftps");
      auto dir = req.GetKey("dir");
      auto DCProt = req.GetKey("dcprot");

      auto ftp = NPL::make_ftp(host, std::stoi(port), NPL::TLS::NO);

      ftp->SetCredentials(user, pass);

      ftp->StartClient();

      if (dir.length())
      {
        GetDirectoryList(c, ftp, dir);
      }
    }

    void GetDirectoryList(SPCProtocol c, SPCProtocol p, const std::string& dir)
    {
      NPL::DCProt prot = NPL::DCProt::Protected;

      auto ftp = std::dynamic_pointer_cast<CProtocolFTP>(p);

      ftp->ListDirectory(
        [list = std::string(""), c, dir, this] 
        (const char *b, size_t n) mutable {
          if (b) 
          {
            list.append(std::string(b, n));
          }
          else
          {
            Json res;

            res.SetKey("data", list);
            res.SetKey("dir", dir);            
            res.SetKey("event", "list");
            res.SetKey("service", "ftp");

            SendResponse(c, res);
          }
          return true;
        }, dir, prot);
    }

};

using SPCFTPManager = std::shared_ptr<CFTPManager>;
using WPCFTPManager = std::weak_ptr<CFTPManager>;

#endif //FTPMANAGER_HPP