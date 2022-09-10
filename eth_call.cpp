#include "eth_call.h"

#include "core/io/json.h"
#include "scene/main/http_request.h"

Error EthCall::request(const String &p_url, const String &p_name, const Array &p_inputs) {
  ERR_FAIL_COND_V(!abi.is_valid(), ERR_UNCONFIGURED);

  Dictionary call;
  call["to"] = to;
  call["from"] = from;
  call["data"] = abi->encode_function_inputs(p_name, p_inputs);

  Array params;
  params.push_back(call);
  params.push_back("latest");

  Dictionary data;
  data["id"] = 1;
  data["jsonrpc"] = "2.0";
  data["method"] = "eth_call";
  data["params"] = params;

  return request_call->request(p_url, Vector<String>(), true, HTTPClient::METHOD_POST, JSON::print(data));
}

void EthCall::_request_completed(int p_status, int p_code, const PoolStringArray &headers, const PoolByteArray &p_data) {
  if (p_status != HTTPRequest::RESULT_SUCCESS || p_code != 200) {
    // TODO call signal error
    return;
  }

  String response_json;
  {
    PoolByteArray::Read r = p_data.read();
    response_json.parse_utf8((const char *)r.ptr(), p_data.size());
  }

  Variant json;
  String errs;
  int errline;
  Error err = JSON::parse(response_json, json, errs, errline);
  if (err != OK) {
    // TODO call signal error
    return;
  }

  emit_signal("call_completed", json);
}

void EthCall::set_to(const String &p_address) {
  to = p_address;
}

String EthCall::get_to() const {
  return to;
}

void EthCall::set_from(const String &p_address) {
  from = p_address;
}

String EthCall::get_from() const {
  return from;
}

void EthCall::set_abi(const Ref<ABI> &p_abi) {
  abi = p_abi;
}

Ref<ABI> EthCall::get_abi() const {
  return abi;
}

void EthCall::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_abi", "abi"), &EthCall::set_abi);
  ClassDB::bind_method(D_METHOD("get_abi"), &EthCall::get_abi);

  ClassDB::bind_method(D_METHOD("set_to", "to"), &EthCall::set_to);
  ClassDB::bind_method(D_METHOD("get_to"), &EthCall::get_to);

  ClassDB::bind_method(D_METHOD("set_from", "from"), &EthCall::set_from);
  ClassDB::bind_method(D_METHOD("get_from"), &EthCall::get_from);

  ClassDB::bind_method(D_METHOD("request", "url", "name", "inputs"), &EthCall::request);
  ClassDB::bind_method("_request_completed", &EthCall::_request_completed);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "abi", PROPERTY_HINT_RESOURCE_TYPE, "ABI"), "set_abi", "get_abi");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "to"), "set_to", "get_to");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "from"), "set_from", "get_from");

  ADD_SIGNAL(MethodInfo("call_completed", PropertyInfo(Variant::DICTIONARY, "result")));
}

EthCall::EthCall() {
  request_call = memnew(HTTPRequest);
  add_child(request_call);
  request_call->connect("request_completed", this, "_request_completed");
}