#include "eth_balance.h"

Error EthBalance::request(const String &p_address) {
  Array params;
  params.push_back(p_address);
  params.push_back("latest");

  return rpc_request->request("eth_getBalance", params);
}

void EthBalance::_request_completed(int p_status, const Dictionary &p_result) {
  emit_signal("request_completed", p_status, p_result["result"]);
}

void EthBalance::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "address"), &EthBalance::request);
  ClassDB::bind_method("_request_completed", &EthBalance::_request_completed);

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::STRING, "result")));
}

EthBalance::EthBalance() {
  rpc_request = memnew(RPCRequest);
  add_child(rpc_request);
  rpc_request->connect("request_completed", this, "_request_completed");
}