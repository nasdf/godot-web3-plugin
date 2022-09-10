#ifndef ETH_CALL_H
#define ETH_CALL_H

#include "scene/main/node.h"

#include "abi.h"

class HTTPRequest;

class EthCall : public Node {
  GDCLASS(EthCall, Node);

  Ref<ABI> abi;
  String to;
  String from;
  
  HTTPRequest *request_call = nullptr;
  void _request_completed(int p_status, int p_code, const PoolStringArray &headers, const PoolByteArray &p_data);

protected:
  static void _bind_methods();

public:
  void set_to(const String &p_address);
  String get_to() const;

  void set_from(const String &p_address);
  String get_from() const;

  void set_abi(const Ref<ABI> &p_abi);
  Ref<ABI> get_abi() const;

  Error request(const String &p_url, const String &p_name, const Array &p_inputs);

  EthCall();
};

#endif // ETH_CALL_H