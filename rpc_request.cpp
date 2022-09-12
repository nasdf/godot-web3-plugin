#include "rpc_request.h"

#include "core/io/json.h"
#include "core/project_settings.h"
#include "scene/main/http_request.h"

Error RPCRequest::request(const String &p_method, const Array &p_params) {
  String url = GLOBAL_GET("web3/rpc_url");
  ERR_FAIL_COND_V_MSG(url.empty(), ERR_UNCONFIGURED, "RPC URL is empty in project settings.");

  Dictionary data;
  data["id"] = 1;
  data["jsonrpc"] = "2.0";
  data["method"] = p_method;
  data["params"] = p_params;

  return http_request->request(url, Vector<String>(), true, HTTPClient::METHOD_POST, JSON::print(data));
}

void RPCRequest::_request_completed(int p_status, int p_code, const PoolStringArray &headers, const PoolByteArray &p_data) {
  if (p_status != HTTPRequest::RESULT_SUCCESS || p_code != 200) {
    emit_signal("request_completed", RESULT_HTTP_ERROR, Dictionary());
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
    emit_signal("request_completed", RESULT_JSON_ERROR, Dictionary());
    return;
  }

  emit_signal("request_completed", RESULT_SUCCESS, json);
}

void RPCRequest::_bind_methods() {
  ClassDB::bind_method(D_METHOD("request", "method", "params"), &RPCRequest::request);
  ClassDB::bind_method("_request_completed", &RPCRequest::_request_completed);

  ADD_SIGNAL(MethodInfo("request_completed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::DICTIONARY, "result")));

  BIND_ENUM_CONSTANT(RESULT_SUCCESS);
  BIND_ENUM_CONSTANT(RESULT_HTTP_ERROR);
  BIND_ENUM_CONSTANT(RESULT_JSON_ERROR);
}

RPCRequest::RPCRequest() {
  http_request = memnew(HTTPRequest);
  add_child(http_request);
  http_request->connect("request_completed", this, "_request_completed");
}