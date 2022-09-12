#include "eth_call.h"

Error EthCall::request(const String &p_to, const String &p_from, const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V_MSG(!abi.is_valid(), ERR_UNCONFIGURED, "ABI is undefined.");

  Dictionary call;
  call["to"] = p_to;
  call["from"] = p_from;
  call["data"] = abi->encode_function_inputs(p_name, p_inputs);

  Array params;
  params.push_back(call);
  params.push_back("latest");

  return rpc_request->request("eth_call", params);
}

void EthCall::_request_completed(int p_status, const Dictionary &p_result) {
  // TODO ABI decode results

  emit_signal("request_completed", p_status, p_result);
}

void EthCall::set_abi(const Ref<ABI> &p_abi) {
  abi = p_abi;
}

Ref<ABI> EthCall::get_abi() const {
  return abi;
}

void EthCall::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "name", "inputs"), &EthCall::request);
  ClassDB::bind_method("_request_completed", &EthCall::_request_completed);

  ClassDB::bind_method(D_METHOD("set_abi", "abi"), &EthCall::set_abi);
  ClassDB::bind_method(D_METHOD("get_abi"), &EthCall::get_abi);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "abi", PROPERTY_HINT_RESOURCE_TYPE, "ABI"), "set_abi", "get_abi");

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::DICTIONARY, "result")));
}

EthCall::EthCall() {
  rpc_request = memnew(RPCRequest);
  add_child(rpc_request);
  rpc_request->connect("request_completed", this, "_request_completed");
}