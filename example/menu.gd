extends Control

func _ready():
	var wallet = Wallet.new()
	wallet.set_private_key("")
	
	$EthTransaction.connect("request_completed", self, "_request_completed")
	$EthTransaction.request("mint", ["0x00"], wallet);

func _request_completed(status, result):
	print(result)
