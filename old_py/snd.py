from raspi_lora import LoRa, ModemConfig

ch = int(input())
if (ch == 0):
	pin = 25
elif (ch == 1): 
	pin = 16
else: 
	pin = 0 

print(f'Ch: {ch}, Pin: {pin}')
lora = LoRa(ch, pin, 2, modem_config=ModemConfig.Bw125Cr45Sf128, tx_power=14, acks=True)
lora.set_mode_tx()
msgId = 0
msg = "Hello LoRa\n"
while (True):
	lora.send_to_wait(msg, 3, retries=10)
	msgId += 1
	msgId %= 256

