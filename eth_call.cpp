#include "eth_call.h"

Error EthCall::request(const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V_MSG(!contract_abi.is_valid(), ERR_UNCONFIGURED, "Contract ABI is undefined.");
  ERR_FAIL_COND_V_MSG(contract_address.empty(), ERR_UNCONFIGURED, "Contract address is empty.");

  function_name = p_name;

  Dictionary call;
  call["to"] = contract_address;
  call["from"] = "0x0000000000000000000000000000000000000000";
  call["data"] = contract_abi->encode_function(p_name, p_inputs);

  Array params;
  params.push_back(call);
  params.push_back("latest");

  return rpc_request->request("eth_call", params);
}

void EthCall::_request_completed(int p_status, const Dictionary &p_result) {
  Array result = contract_abi->decode_function(function_name, p_result["result"]);
  emit_signal("request_completed", p_status, result);
}

void EthCall::set_contract_abi(const Ref<ABI> &p_abi) {
  contract_abi = p_abi;
}

Ref<ABI> EthCall::get_contract_abi() const {
  return contract_abi;
}

void EthCall::set_contract_address(const String &p_address) {
  contract_address = p_address;
}

String EthCall::get_contract_address() const {
  return contract_address;
}

void EthCall::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "name", "inputs"), &EthCall::request);
  ClassDB::bind_method("_request_completed", &EthCall::_request_completed);

  ClassDB::bind_method(D_METHOD("set_contract_abi", "abi"), &EthCall::set_contract_abi);
  ClassDB::bind_method(D_METHOD("get_contract_abi"), &EthCall::get_contract_abi);

  ClassDB::bind_method(D_METHOD("set_contract_address", "address"), &EthCall::set_contract_address);
  ClassDB::bind_method(D_METHOD("get_contract_address"), &EthCall::get_contract_address);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "contract_abi", PROPERTY_HINT_RESOURCE_TYPE, "ABI"), "set_contract_abi", "get_contract_abi");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "contract_address"), "set_contract_address", "get_contract_address");

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::ARRAY, "result")));
}

EthCall::EthCall() {
  rpc_request = memnew(RPCRequest);
  add_child(rpc_request);
  rpc_request->connect("request_completed", this, "_request_completed");
}