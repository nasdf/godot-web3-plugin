#ifndef ETH_TRANSACTION
#define ETH_TRANSACTION

#include "scene/main/node.h"

#include "abi.h"
#include "rpc_request.h"

class EthTransaction : public Node {
  GDCLASS(EthTransaction, Node);

  enum State {
    STATE_IDLE,
    STATE_NONCE,
    STATE_CHAIN_ID,
    STATE_GAS_PRICE,
    STATE_GAS_ESTIMATE,
    STATE_SEND_TRANSACTION
  };

  State state;
  Dictionary tx;
  Ref<ABI> abi;

  RPCRequest *rpc_request = nullptr;
  Error _request(const Variant &p_result);
  void _request_completed(int p_status, const Dictionary &p_result);
  
protected:
  static void _bind_methods();

public:
  void set_abi(const Ref<ABI> &p_abi);
  Ref<ABI> get_abi() const;

  Error request(const String &p_to, const String &p_from, const String &p_name, const Array &p_inputs);

  EthTransaction();
};

#endif // ETH_TRANSACTION