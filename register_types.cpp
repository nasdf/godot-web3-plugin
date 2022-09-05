#include "register_types.h"
#include "eth_call.h"
#include "keccak.h"

#include "core/class_db.h"

void register_web3_types() {
   ClassDB::register_class<Keccak>();
   ClassDB::register_class<EthCall>();
}

void unregister_web3_types() {
   // do nothing
}