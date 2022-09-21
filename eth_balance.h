#ifndef ETH_BALANCE_H
#define ETH_BALANCE_H

#include "scene/main/node.h"

#include "rpc_request.h"

class EthBalance : public Node {
  GDCLASS(EthBalance, Node);

  RPCRequest *rpc_request = nullptr;
  void _request_completed(int p_status, const Dictionary &p_result);
  
protected:
  static void _bind_methods();

public:
  Error request(const String &p_address);

  EthBalance();
};

#endif // ETH_BALANCE_H