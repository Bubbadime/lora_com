test: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c main.c -Wall -lwiringPi -o ./bin/test

fsnd: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c fsnd.c -Wall -lwiringPi -o ./bin/fsnd

frcv: main.c PlatformLoRa.c LoRa.c
	mkdir -p bin
	gcc PlatformLoRa.c LoRa.c frcv.c -Wall -lwiringPi -o ./bin/frcv

