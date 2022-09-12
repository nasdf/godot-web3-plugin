#include "eth_transaction.h"

Error EthTransaction::request(const String &p_to, const String &p_from, const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V_MSG(!abi.is_valid(), ERR_UNCONFIGURED, "ABI is undefined.");
  ERR_FAIL_COND_V_MSG(state != STATE_IDLE, ERR_BUSY, "Transaction in progress.");

  tx.clear();
  tx["to"] = p_to;
  tx["from"] = p_from;
  tx["data"] = abi->encode_function_inputs(p_name, p_inputs);

  return _request(p_from);
}

Error EthTransaction::_request(const Variant &p_result) {
  switch (state) {
    case STATE_IDLE: {
      state = STATE_NONCE;

      Array params;
      params.push_back(p_result);
      params.push_back("latest");

      return rpc_request->request("eth_getTransactionCount", params);
    }
    case STATE_NONCE:
      tx["nonce"] = p_result;
      state = STATE_CHAIN_ID;
      return rpc_request->request("eth_chainId");
    case STATE_CHAIN_ID:
      tx["chainId"] = p_result;
      state = STATE_GAS_PRICE;
      return rpc_request->request("eth_gasPrice");
    case STATE_GAS_PRICE: {
      tx["gasPrice"] = p_result;
      state = STATE_GAS_ESTIMATE;
      
      Array params;
      params.push_back(tx);

      return rpc_request->request("eth_estimateGas", params);
    } 
    case STATE_GAS_ESTIMATE:
      tx["gasLimit"] = p_result;
      state = STATE_SEND_TRANSACTION;

      // TODO sign transaction
      // TODO eth_sendRawTransaction

      return rpc_request->request("eth_sendRawTransaction");
    default:
      return ERR_BUG;
  }
}

void EthTransaction::_request_completed(int p_status, const Dictionary &p_result) {
  if (p_status != RPCRequest::RESULT_SUCCESS) {
    state = STATE_IDLE;
    emit_signal("request_completed", p_status, Dictionary());
    return;
  }

  if (state == STATE_SEND_TRANSACTION) {
    state = STATE_IDLE;
    emit_signal("request_completed", p_status, p_result);
    return;
  }

  Error err = _request(p_result["result"]);
  if (err != OK) {
    state = STATE_IDLE;
    emit_signal("request_completed", RPCRequest::RESULT_HTTP_ERROR, Dictionary());
  }
}

void EthTransaction::set_abi(const Ref<ABI> &p_abi) {
  abi = p_abi;
}

Ref<ABI> EthTransaction::get_abi() const {
  return abi;
}

void EthTransaction::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "name", "inputs"), &EthTransaction::request);
  ClassDB::bind_method("_request_completed", &EthTransaction::_request_completed);

  ClassDB::bind_method(D_METHOD("set_abi", "abi"), &EthTransaction::set_abi);
  ClassDB::bind_method(D_METHOD("get_abi"), &EthTransaction::get_abi);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "abi", PROPERTY_HINT_RESOURCE_TYPE, "ABI"), "set_abi", "get_abi");

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::DICTIONARY, "result")));
}

EthTransaction::EthTransaction() {
  state = STATE_IDLE;

  rpc_request = memnew(RPCRequest);
  add_child(rpc_request);
  rpc_request->connect("request_completed", this, "_request_completed");
}