#include "transaction.h"

void Transaction::set_nonce(const String &p_nonce) {
  nonce = p_nonce;
}

String Transaction::get_nonce() const {
  return nonce;
}

void Transaction::set_gas_price(const String &p_gas_price) {
  gas_price = p_gas_price;
}

String Transaction::get_gas_price() const {
  return gas_price;
}

void Transaction::set_gas_limit(const String &p_gas_limit) {
  gas_limit = p_gas_limit;
}

String Transaction::get_gas_limit() const {
  return gas_limit;
}

void Transaction::set_to(const String &p_to) {
  to = p_to;
}

String Transaction::get_to() const {
  return to;
}

void Transaction::set_value(const String &p_value) {
  value = p_value;
}

String Transaction::get_value() const {
  return value;
}

void Transaction::set_data(const String &p_data) {
  data = p_data;
}

String Transaction::get_data() const {
  return data;
}

void Transaction::set_chain_id(const String &p_chain_id) {
  chain_id = p_chain_id;
}

String Transaction::get_chain_id() const {
  return chain_id;
}

PoolByteArray Transaction::encode() {
  PoolByteArray list;
  list.append_array(rlp.encode(nonce));
  list.append_array(rlp.encode(gas_price));
  list.append_array(rlp.encode(gas_limit));
  list.append_array(rlp.encode(to));
  list.append_array(rlp.encode(value));
  list.append_array(rlp.encode(data));
  list.append_array(rlp.encode(chain_id));
  list.append_array(rlp.encode(r));
  list.append_array(rlp.encode(s));

  PoolByteArray out;
  out.append_array(rlp.encode_length(list.size(), 192));
  out.append_array(list);
  
  return out;
}

void Transaction::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_chain_id", "chain_id"), &Transaction::set_chain_id);
  ClassDB::bind_method(D_METHOD("get_chain_id"), &Transaction::get_chain_id);

  ClassDB::bind_method(D_METHOD("set_nonce", "nonce"), &Transaction::set_nonce);
  ClassDB::bind_method(D_METHOD("get_nonce"), &Transaction::get_nonce);

  ClassDB::bind_method(D_METHOD("set_gas_price", "gas_price"), &Transaction::set_gas_price);
  ClassDB::bind_method(D_METHOD("get_gas_price"), &Transaction::get_gas_price);

  ClassDB::bind_method(D_METHOD("set_gas_limit", "gas_limit"), &Transaction::set_gas_limit);
  ClassDB::bind_method(D_METHOD("get_gas_limit"), &Transaction::get_gas_limit);

  ClassDB::bind_method(D_METHOD("set_to", "to"), &Transaction::set_to);
  ClassDB::bind_method(D_METHOD("get_to"), &Transaction::get_to);

  ClassDB::bind_method(D_METHOD("set_value", "value"), &Transaction::set_value);
  ClassDB::bind_method(D_METHOD("get_value"), &Transaction::get_value);

  ClassDB::bind_method(D_METHOD("set_data", "data"), &Transaction::set_data);
  ClassDB::bind_method(D_METHOD("get_data"), &Transaction::get_data);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "chain_id"), "set_chain_id", "get_chain_id");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "nonce"), "set_nonce", "get_nonce");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "gas_price"), "set_gas_price", "get_gas_price");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "gas_limit"), "set_gas_limit", "get_gas_limit");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "to"), "set_to", "get_to");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "value"), "set_value", "get_value");
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "data"), "set_data", "get_data");
}