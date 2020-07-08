#ifndef MANAGER_HPP
#define MANAGER_HPP

#include <memory>
#include <vector>

#include <npl.hpp>

class CManager
{
  public:

    CManager() {}

    ~CManager() {}

    virtual void Dispatch(SPCProtocol c, Json& req) = 0;

    void SendResponse(SPCProtocol c, Json& res)
    {
      auto message = res.Stringify();

      c->SendProtocolMessage(
          (uint8_t *)message.data(), 
          message.size()
        );   
    }

  protected:

    std::vector<SPCProtocol> iSessions;
};

using SPCManager = std::shared_ptr<CManager>;
using WPCManager = std::weak_ptr<CManager>;

#endif //MANAGER_HPP