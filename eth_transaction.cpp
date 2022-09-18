#include "eth_transaction.h"

Error EthTransaction::request(const String &p_name, const Array &p_inputs, Ref<Wallet> p_wallet) {
  ERR_FAIL_COND_V_MSG(!p_wallet.is_valid(), ERR_UNCONFIGURED, "Wallet is undefined.");
  ERR_FAIL_COND_V_MSG(!contract_abi.is_valid(), ERR_UNCONFIGURED, "Contract ABI is undefined.");
  ERR_FAIL_COND_V_MSG(contract_address.empty(), ERR_UNCONFIGURED, "Contract address is empty.");

  wallet = p_wallet;

  transaction = memnew(Transaction);  
  transaction->set_to(contract_address);
  transaction->set_data(contract_abi->encode_function_inputs(p_name, p_inputs));

  Array params;
  params.push_back(p_wallet->get_address());
  params.push_back("latest");

  return nonce_request->request("eth_getTransactionCount", params);
}

void EthTransaction::_nonce_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  transaction->set_nonce(p_result["result"]);
  Error err = chain_id_request->request("eth_chainId");
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }
}

void EthTransaction::_chain_id_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  transaction->set_chain_id(p_result["result"]);
  Error err = gas_price_request->request("eth_gasPrice");
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }
}

void EthTransaction::_gas_price_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  transaction->set_gas_price(p_result["result"]);

  Dictionary tx;
  tx["from"] = wallet->get_address();
  tx["to"] = transaction->get_to();
  tx["data"] = transaction->get_data();

  Array params;
  params.push_back(tx);

  Error err = estimate_gas_request->request("eth_estimateGas", params);
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }
}

void EthTransaction::_estimate_gas_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  transaction->set_gas_limit(p_result["result"]);
  Error err = wallet->sign_transaction(*transaction);
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }

  PoolByteArray rlp = transaction->encode();
  String rlp_hex = String::hex_encode_buffer(rlp.read().ptr(), rlp.size());

  Array params;
  params.push_back("0x" + rlp_hex);

  err = send_raw_tx_request->request("eth_sendRawTransaction", params);
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }
}

void EthTransaction::_send_raw_tx_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  transaction->set_hash(p_result["result"]);
  get_tx_receipt_timer->start();
}

void EthTransaction::_get_tx_receipt_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    return _request_completed(p_status);
  }

  Dictionary receipt = p_result["result"];
  if (!receipt.empty()) {
    return _request_completed(p_status, p_result);
  }

  get_tx_receipt_timer->start();
}

void EthTransaction::_request_completed(int p_status, const Dictionary &p_result) {
  memdelete(transaction);
  transaction = nullptr;

  emit_signal("request_completed", p_status, p_result);
}

void EthTransaction::_get_tx_receipt_timeout() {
  Array params;
  params.push_back(transaction->get_hash());

  Error err = get_tx_receipt_request->request("eth_getTransactionReceipt", params);
  if (err != OK) {
    return _request_completed(RPCRequest::RESULT_HTTP_ERROR);
  }
}

void EthTransaction::set_contract_abi(const Ref<ABI> &p_abi) {
  contract_abi = p_abi;
}

Ref<ABI> EthTransaction::get_contract_abi() const {
  return contract_abi;
}

void EthTransaction::set_contract_address(const String &p_address) {
  contract_address = p_address;
}

String EthTransaction::get_contract_address() const {
  return contract_address;
}

void EthTransaction::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "name", "inputs", "wallet"), &EthTransaction::request);
  ClassDB::bind_method("_nonce_request_completed", &EthTransaction::_nonce_request_completed);
  ClassDB::bind_method("_chain_id_request_completed", &EthTransaction::_chain_id_request_completed);
  ClassDB::bind_method("_gas_price_request_completed", &EthTransaction::_gas_price_request_completed);
  ClassDB::bind_method("_estimate_gas_request_completed", &EthTransaction::_estimate_gas_request_completed);
  ClassDB::bind_method("_send_raw_tx_request_completed", &EthTransaction::_send_raw_tx_request_completed);
  ClassDB::bind_method("_get_tx_receipt_request_completed", &EthTransaction::_get_tx_receipt_request_completed);
  ClassDB::bind_method("_get_tx_receipt_timeout", &EthTransaction::_get_tx_receipt_timeout);

  ClassDB::bind_method(D_METHOD("set_contract_abi", "abi"), &EthTransaction::set_contract_abi);
  ClassDB::bind_method(D_METHOD("get_contract_abi"), &EthTransaction::get_contract_abi);

  ClassDB::bind_method(D_METHOD("set_contract_address", "address"), &EthTransaction::set_contract_address);
  ClassDB::bind_method(D_METHOD("get_contract_address"), &EthTransaction::get_contract_address);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "contract_abi", PROPERTY_HINT_RESOURCE_TYPE, "ABI"), "set_contract_abi", "get_contract_abi");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "contract_address"), "set_contract_address", "get_contract_address");

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::DICTIONARY, "result")));
}

EthTransaction::EthTransaction() {
  nonce_request = memnew(RPCRequest);
  add_child(nonce_request);
  nonce_request->connect("request_completed", this, "_nonce_request_completed");

  chain_id_request = memnew(RPCRequest);
  add_child(chain_id_request);
  chain_id_request->connect("request_completed", this, "_chain_id_request_completed");

  gas_price_request = memnew(RPCRequest);
  add_child(gas_price_request);
  gas_price_request->connect("request_completed", this, "_gas_price_request_completed");

  estimate_gas_request = memnew(RPCRequest);
  add_child(estimate_gas_request);
  estimate_gas_request->connect("request_completed", this, "_estimate_gas_request_completed");

  send_raw_tx_request = memnew(RPCRequest);
  add_child(send_raw_tx_request);
  send_raw_tx_request->connect("request_completed", this, "_send_raw_tx_request_completed");

  get_tx_receipt_request = memnew(RPCRequest);
  add_child(get_tx_receipt_request);
  get_tx_receipt_request->connect("request_completed", this, "_get_tx_receipt_request_completed");

  get_tx_receipt_timer = memnew(Timer);
  add_child(get_tx_receipt_timer);
  get_tx_receipt_timer->set_one_shot(true);
  get_tx_receipt_timer->set_wait_time(3.0);
  get_tx_receipt_timer->connect("timeout", this, "_get_tx_receipt_timeout");
}