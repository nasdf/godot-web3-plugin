extends Control

onready var address_value = get_node("Account/Address/Value")
onready var balance_value = get_node("Account/Balance/Value")

onready var cookie_count = get_node("Inventory/GridContainer/Cookie/Count")
onready var apple_count = get_node("Inventory/GridContainer/Apple/Count")
onready var cheese_count = get_node("Inventory/GridContainer/Cheese/Count")
onready var potato_count = get_node("Inventory/GridContainer/Potato/Count")

var wallet = Wallet.new()
var token_id = ""

func _ready():
	wallet.set_private_key("0x87237087a178155089c647f541071f7f636b2a3942b2fb52eb5ade8cfcc0c132")
	address_value.text = wallet.get_address().substr(0, 6) + ".." + wallet.get_address().substr(38)
	
func _update_state():
	var address = wallet.get_address()
	var addresses = [address, address, address, address]
	var token_ids = ["0x00", "0x01", "0x02", "0x03"]
	
	$EthCall.request("balanceOfBatch", [addresses, token_ids])
	$EthBalance.request(address)
	
func _purchase_cookie():
	$Confirm.popup()
	token_id = "0x00"
	
func _purchase_apple():
	$Confirm.popup()
	token_id = "0x01"
	
func _purchase_cheese():
	$Confirm.popup()
	token_id = "0x02"
	
func _purchase_potato():
	$Confirm.popup()
	token_id = "0x03"
	
func _confirm_no():
	$Confirm.hide()
	
func _confirm_yes():
	$Confirm.hide()
	$Loading.popup()
	$EthTransaction.request("mint", [token_id], wallet)

func _call_completed(status, result):
	var value = result[0] as Array
	cookie_count.text = str(value[0])
	apple_count.text = str(value[1])
	cheese_count.text = str(value[2])
	potato_count.text = str(value[3])

func _balance_completed(status, result):
	var balance = result.hex_to_int() / pow(10, 18)
	balance_value.text = str(balance)

func _transaction_completed(status, result):
	$Loading.hide()
	_update_state()
