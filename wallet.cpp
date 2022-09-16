#include "wallet.h"

#include "keccak.h"

#include "thirdparty/trezor-crypto/ecdsa.h"
#include "thirdparty/trezor-crypto/secp256k1.h"

String Wallet::get_address() const {
  return address;
}

Error Wallet::set_private_key(const PoolByteArray &p_private_key) {
  ERR_FAIL_COND_V_MSG(p_private_key.size() != 32, ERR_INVALID_PARAMETER, "Private key must be 32 bytes.");
  memcpy(private_key, p_private_key.read().ptr(), 32);
  
  // get the uncompressed public key
  uint8_t public_key[65];
  ecdsa_get_public_key65(&secp256k1, private_key, public_key);

  // hash of x and y coordinate without 04 prefix
  unsigned char hash[32];
  keccak_256(public_key + 1, 64, hash);
  address = String::hex_encode_buffer(hash + 12, 20);

  return OK;
}

Error Wallet::sign_transaction(Transaction &p_transaction) {
  PoolByteArray rlp = p_transaction.encode();

  uint8_t hash[32];
  Keccak::hash(rlp.read().ptr(), rlp.size(), hash);

  uint8_t pby;
  uint8_t sig[64];

  int ret = ecdsa_sign_digest(&secp256k1, private_key, hash, sig, &pby, nullptr);
  ERR_FAIL_COND_V(ret, FAILED);

  // EIP 155 replay protection
  int chain_id = p_transaction.get_chain_id().hex_to_int(true);
  int v = 2 * chain_id + (pby % 2) + pby;

  p_transaction.set_r(String::hex_encode_buffer(sig, 32));
  p_transaction.set_s(String::hex_encode_buffer(sig + 32, 32));
  p_transaction.set_v(String::num_int64(v, 16));

  return OK;
}

void Wallet::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_address"), &Wallet::get_address);
  ClassDB::bind_method(D_METHOD("set_private_key", "private_key"), &Wallet::set_private_key);
}
