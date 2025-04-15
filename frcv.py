from raspi_lora import LoRa, ModemConfig
from time import sleep
import sys
import numpy as np

global running
global last
global buf
global imgId

running = True
last = -1
buf = bytearray()
imgId = -1

def end_recv():
	global running
	running = False

def on_recv(payload):
	global last
	global imgId
	snr = payload.snr
	snr = snr * 4
	snr = np.int8(snr)
	fsnr = snr / 4
	id = int.from_bytes(payload.message[0:4])
	if (imgId == -1):
		imgId = id
	if (payload.message == b'END_OF_MESSAGE'):
		print('EOM!')
		end_recv()	
		return
	print(f"MessageLen: {len(payload.message)}, header_to={payload.header_to}, header_from={payload.header_from}, header_id={payload.header_id}, rssi={payload.rssi}, snr={fsnr}, imageId={id}") 
	if (running):
		if (payload.header_id < last or not id == imgId):
			print('Cur: {payload.header_id}, last: {last}')
			end_recv()
		elif (payload.header_id > last):
			global buf
			buf = buf + payload.message[4::]
			last = payload.header_id

try:
	ch = int(sys.argv[1])
	if (ch == 0):
		pin = 25
	elif (ch == 1): 
		pin = 16
	else: 
		pin = 0 
	print(f'Ch: {ch}, Pin: {pin}')
	lora = LoRa(ch, pin, 3, modem_config=ModemConfig.Bw125Cr45Sf128, tx_power=23, acks=True)
	lora.on_recv = on_recv 
	lora.set_mode_rx()
	while (running):
		sleep(0.0)
except Exception:
	end_recv()	
finally:
	print('Ending message recv')
	print(last)
	if (len(buf) > 255):
		fi = open("index.txt", "r")
		lastId = int(fi.read())
		fi.close()
		if (lastId == imgId):
			f = open("out.ssdv", "ab")
		else:
			f = open("out.ssdv", "wb")
		fi = open("index.txt", "w")
		fi.write(str(imgId))
		f.write(buf)
		f.close()
		fi.close()

	exit()

