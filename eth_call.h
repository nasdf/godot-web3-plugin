#ifndef ETH_CALL_H
#define ETH_CALL_H

#include "scene/main/node.h"

class HTTPRequest;

class EthCall : public Node {
  GDCLASS(EthCall, Node);

private:
  HTTPRequest *request_call = nullptr;

  void _request_completed(int p_status, int p_code, const PoolStringArray &headers, const PoolByteArray &p_data);

protected:
  static void _bind_methods();

public:
  Error request(const String &p_url, const String &p_request_data);

  EthCall();
};

#endif // ETH_CALL_H