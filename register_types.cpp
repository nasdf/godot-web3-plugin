#include "register_types.h"

#include "abi.h"
#include "eth_balance.h"
#include "eth_call.h"
#include "eth_transaction.h"
#include "keccak.h"
#include "rlp.h"
#include "rpc_request.h"
#include "transaction.h"
#include "wallet.h"

#include "core/class_db.h"
#include "core/project_settings.h"

static Ref<ResourceFormatLoaderABI> abi_loader;

void register_web3_types() {
   ClassDB::register_class<ABI>();
   ClassDB::register_class<EthBalance>();
   ClassDB::register_class<EthCall>();
   ClassDB::register_class<EthTransaction>();
   ClassDB::register_class<Keccak>();
   ClassDB::register_class<RLP>();
   ClassDB::register_class<RPCRequest>();
   ClassDB::register_class<Transaction>();
   ClassDB::register_class<Wallet>();

   abi_loader.instance();
   ResourceLoader::add_resource_format_loader(abi_loader);

   GLOBAL_DEF("web3/rpc_url", "");
}

void unregister_web3_types() {
   ResourceLoader::remove_resource_format_loader(abi_loader);
   abi_loader.unref();
}