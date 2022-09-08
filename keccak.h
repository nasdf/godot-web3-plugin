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
  Keccak();
  ~Keccak();

  Error update(PoolByteArray p_chunk);
  PoolByteArray finish();
};

#endif // KECCAK_H