#include "eth_call.h"

#include "core/io/json.h"
#include "scene/main/http_request.h"

Error EthCall::request(const String &p_url, const String &p_request_data) {
  // TODO ABI encode request data

  return request_call->request(p_url, Vector<String>(), true, HTTPClient::METHOD_POST, p_request_data);
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

void EthCall::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "url", "request_data"), &EthCall::request);

  ADD_SIGNAL(MethodInfo("call_completed", PropertyInfo(Variant::DICTIONARY, "result")));
}

EthCall::EthCall() {
  request_call = memnew(HTTPRequest);
  add_child(request_call);
  request_call->connect("request_completed", this, "_request_completed");
}