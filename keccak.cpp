#include "keccak.h"

#include "thirdparty/trezor-crypto/sha3.h"

Keccak::Keccak() {
  ctx = memalloc(sizeof(SHA3_CTX));
  keccak_256_Init((SHA3_CTX *)ctx);
}

Keccak::~Keccak() {
  memfree((SHA3_CTX *)ctx);
}

Error Keccak::update(PoolByteArray p_chunk) {
  ERR_FAIL_COND_V(ctx == nullptr, ERR_UNCONFIGURED);
  size_t len = p_chunk.size();
  ERR_FAIL_COND_V(len == 0, FAILED);
  PoolByteArray::Read r = p_chunk.read();
  keccak_Update((SHA3_CTX *)ctx, &r[0], len);
  return OK;
}

PoolByteArray Keccak::finish() {
  ERR_FAIL_COND_V(ctx == nullptr, PoolByteArray());
  PoolByteArray out;
  out.resize(32);
  keccak_Final((SHA3_CTX *)ctx, out.write().ptr());
  return out;
}

void Keccak::_bind_methods() {
  ClassDB::bind_method(D_METHOD("update", "chunk"), &Keccak::update);
  ClassDB::bind_method(D_METHOD("finish"), &Keccak::finish);
}
