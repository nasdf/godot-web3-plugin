#include "wallet.h"

#include "thirdparty/trezor-crypto/secp256k1.h"
#include "thirdparty/trezor-crypto/ecdsa.h"

String Wallet::get_address() const {
  return address;
}

Error Wallet::set_private_key(const PoolByteArray &p_private_key) {
  ERR_FAIL_COND_V_MSG(p_private_key.size() != 32, ERR_INVALID_PARAMETER, "Private key must be 32 bytes.");
  private_key = p_private_key;
  
  // get the uncompressed public key
  PoolByteArray public_key;
  public_key.resize(65);
  //ecdsa_get_public_key65(&secp256k1, private_key.read().ptr(), public_key.write().ptr());
  ecdsa_get_public_key65(nullptr, nullptr, nullptr);

  // hash of x and y coordinate without 04 prefix
  unsigned char hash[32];
  keccak_256(public_key.read().ptr() + 1, 64, hash);
  address = String::hex_encode_buffer(hash + 12, 20);

  return OK;
}

void Wallet::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_address"), &Wallet::get_address);
  ClassDB::bind_method(D_METHOD("set_private_key", "private_key"), &Wallet::set_private_key);
}