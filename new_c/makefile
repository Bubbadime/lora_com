test: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c main.c -Wall -lwiringPi -o ./bin/test

.phony com: fsnd frcv

fsnd: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c $@.c -Wall -lwiringPi -o ./bin/$@

frcv: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c $@.c -Wall -lwiringPi -o ./bin/$@

