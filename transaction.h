#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "core/reference.h"

#include "rlp.h"

class Transaction : public Reference {
  GDCLASS(Transaction, Reference);

  String nonce;
  String gas_price;
  String gas_limit;
  String to;
  String value;
  String data;
  String chain_id;

  String v;
  String r;
  String s;

  RLP rlp;

protected:
  static void _bind_methods();

public:
  void set_nonce(const String &p_nonce);
  String get_nonce() const;

  void set_gas_price(const String &p_gas_price);
  String get_gas_price() const;

  void set_gas_limit(const String &p_gas_limit);
  String get_gas_limit() const;

  void set_to(const String &p_to);
  String get_to() const;

  void set_value(const String &p_value);
  String get_value() const;

  void set_data(const String &p_data);
  String get_data() const;

  void set_chain_id(const String &p_chain_id);
  String get_chain_id() const;

  void set_v(const String &p_v);
  String get_v() const;

  void set_r(const String &p_r);
  String get_r() const;

  void set_s(const String &p_s);
  String get_s() const;

  PoolByteArray encode();
};

#endif // TRANSACTION_H