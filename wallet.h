#ifndef WALLET_H
#define WALLET_H

#include "core/reference.h"

class Wallet : public Reference {
  GDCLASS(Wallet, Reference);

  String address;
  PoolByteArray private_key;

protected:
  static void _bind_methods();

public:
  String get_address() const;

  Error set_private_key(const PoolByteArray &p_private_key);
};

#endif // WALLET_H