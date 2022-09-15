#ifndef RLP_H
#define RLP_H

#include "core/reference.h"

class RLP : public Reference {
  GDCLASS(RLP, Reference);

  PoolByteArray hex_to_bytes(const String &p_hex);

protected:
  static void _bind_methods();

public:
  PoolByteArray encode_length(int length, int offset);
  PoolByteArray encode(const String &p_hex);
};

#endif // RLP_H