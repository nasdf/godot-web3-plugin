#ifndef RPC_REQUEST_H
#define RPC_REQUEST_H

#include "scene/main/node.h"

class HTTPRequest;

class RPCRequest : public Node {
  GDCLASS(RPCRequest, Node);

  HTTPRequest *http_request = nullptr;
  void _request_completed(int p_status, int p_code, const PoolStringArray &headers, const PoolByteArray &p_data);

protected:
  static void _bind_methods();

public:
  enum Result {
    RESULT_SUCCESS,
    RESULT_HTTP_ERROR,
    RESULT_JSON_ERROR
  };

  Error request(const String &p_method, const Array &p_params = Array());

  RPCRequest();
};

VARIANT_ENUM_CAST(RPCRequest::Result);

#endif // RPC_REQUEST_H