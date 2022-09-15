#ifndef ETH_CALL_H
#define ETH_CALL_H

#include "scene/main/node.h"

#include "abi.h"
#include "rpc_request.h"

class EthCall : public Node {
  GDCLASS(EthCall, Node);

  Ref<ABI> contract_abi;
  String contract_address;

  RPCRequest *rpc_request = nullptr;
  void _request_completed(int p_status, const Dictionary &p_result);
  
protected:
  static void _bind_methods();

public:
  void set_contract_abi(const Ref<ABI> &p_abi);
  Ref<ABI> get_contract_abi() const;

  void set_contract_address(const String &p_address);
  String get_contract_address() const;

  Error request(const String &p_name, const Array &p_inputs);

  EthCall();
};

#endif // ETH_CALL_H