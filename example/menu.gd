extends Control

func _ready():
	var wallet = Wallet.new()
	wallet.set_private_key("0x7c4a968dd1857caa582b50ffe89abeb11ed8cc4cc0828846b548b6307b5c6f39")
	
	$EthTransaction.connect("request_completed", self, "_request_completed")
	$EthTransaction.request("mint", ["0x00"], wallet);

func _request_completed(status, result):
	print(result)
