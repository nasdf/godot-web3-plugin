#ifndef ETH_TRANSACTION
#define ETH_TRANSACTION

#include "scene/main/node.h"
#include "scene/main/timer.h"

#include "abi.h"
#include "rpc_request.h"
#include "transaction.h"
#include "wallet.h"

class EthTransaction : public Node {
  GDCLASS(EthTransaction, Node);

  Ref<ABI> contract_abi;
  String contract_address;

  Ref<Wallet> wallet;
  Transaction *transaction;

  RPCRequest *nonce_request = nullptr;
  RPCRequest *chain_id_request = nullptr;
  RPCRequest *gas_price_request = nullptr;
  RPCRequest *estimate_gas_request = nullptr;
  RPCRequest *send_raw_tx_request = nullptr;
  RPCRequest *get_tx_receipt_request = nullptr;

  void _nonce_request_completed(int p_status, const Dictionary &p_result);
  void _chain_id_request_completed(int p_status, const Dictionary &p_result);
  void _gas_price_request_completed(int p_status, const Dictionary &p_result);
  void _estimate_gas_request_completed(int p_status, const Dictionary &p_result);
  void _send_raw_tx_request_completed(int p_status, const Dictionary &p_result);
  void _get_tx_receipt_request_completed(int p_status, const Dictionary &p_result);
  void _request_completed(int p_status, const Dictionary &p_result = Dictionary());

  Timer *get_tx_receipt_timer;
  void _get_tx_receipt_timeout();
  
protected:
  static void _bind_methods();

public:
  void set_contract_abi(const Ref<ABI> &p_abi);
  Ref<ABI> get_contract_abi() const;

  void set_contract_address(const String &p_address);
  String get_contract_address() const;

  Error request(const String &p_name, const Array &p_inputs, Ref<Wallet> p_wallet);

  EthTransaction();
};

#endif // ETH_TRANSACTION