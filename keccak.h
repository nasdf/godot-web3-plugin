#ifndef KECCAK_H
#define KECCAK_H

#include "core/reference.h"

class Keccak : public Reference {
  GDCLASS(Keccak, Reference);

private:
  void *ctx;

protected:
  static void _bind_methods();

public:
  static void hash(const unsigned char* p_data, int p_len, unsigned char r_digest[32]);

  Error update(PoolByteArray p_chunk);
  PoolByteArray finish();

  Keccak();
  ~Keccak();
};

#endif // KECCAK_H