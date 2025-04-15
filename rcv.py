from raspi_lora import LoRa, ModemConfig
from time import sleep

def on_recv(payload):
	print(payload)

ch = int(input())
if (ch == 0):
	pin = 25
elif (ch == 1): 
	pin = 16
else: 
	pin = 0 

print(f'Ch: {ch}, Pin: {pin}')
lora = LoRa(ch, pin, 3, modem_config=ModemConfig.Bw125Cr45Sf128, tx_power=14, acks=True)
lora.on_recv = on_recv 
lora.set_mode_rx()
while (True):
	sleep(0.1)

