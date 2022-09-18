#include "rlp.h"

#include "string_utils.h"

PoolByteArray RLP::encode_length(int length, int offset) {
  PoolByteArray out;
  if (length < 56) {
    out.append(length + offset);
  } else {
    // convert length to bytes
    String length_hex = String::num_int64(length, 16);
    PoolByteArray length_bin = StringUtils::hex_to_bytes(length_hex);
    // convert prefix to bytes
    int prefix = offset + 55 + length_bin.size();
    String prefix_hex = String::num_int64(prefix, 16);
    PoolByteArray prefix_bin = StringUtils::hex_to_bytes(prefix_hex);
    // append prefix & length bytes
    out.append_array(prefix_bin);
    out.append_array(length_bin);
  }

  return out;
}

PoolByteArray RLP::encode(const String &p_hex) {
  PoolByteArray out;
  PoolByteArray bin = StringUtils::hex_to_bytes(p_hex);
  if (bin.size() == 1 && bin[0] < 128) {
    out.append(bin[0]);
  } else {
    out.append_array(encode_length(bin.size(), 128));
    out.append_array(bin);
  }

  return out;
}

void RLP::_bind_methods() {
  ClassDB::bind_method(D_METHOD("encode", "hex"), &RLP::encode);
  ClassDB::bind_method(D_METHOD("encode_length", "length", "offset"), &RLP::encode_length);
}