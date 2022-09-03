#include "register_types.h"

#include "core/class_db.h"
#include "keccak.h"

void register_web3_types() {
   ClassDB::register_class<Keccak>();
}

void unregister_web3_types() {
   // do nothing
}