#include "register_types.h"

#include "abi.h"
#include "eth_call.h"
#include "keccak.h"

#include "core/class_db.h"

static Ref<ResourceFormatLoaderABI> abi_loader;

void register_web3_types() {
   ClassDB::register_class<ABI>();

   abi_loader.instance();
   ResourceLoader::add_resource_format_loader(abi_loader);

   ClassDB::register_class<Keccak>();
   ClassDB::register_class<EthCall>();
}

void unregister_web3_types() {
   ResourceLoader::remove_resource_format_loader(abi_loader);
   abi_loader.unref();
}