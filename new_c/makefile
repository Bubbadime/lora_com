test: main.c PlatformLoRa.c LoRa.c
	@mkdir -p bin
	gcc PlatformLoRa.c LoRa.c main.c -Wall -lwiringPi -o ./bin/test

LoRaCom: LoRaCom.c PlatformLoRa.c LoRa.c PlatformLoRa.h LoRa.h
	@mkdir -p bin
	gcc PlatformLoRa.c LoRa.c $@.c -Wall -lwiringPi -o ./bin/$@

