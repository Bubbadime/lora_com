// Cstd headers
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// Non Cstd headers
#include <wiringPi.h>
#include <wiringPiSPI.h>

// Custom Headers
#include "LoRa.h"

// Returns a 2-byte sequence to send over SPI
// Writes data to internal LoRa reg if possible
LoRaSingleXfr LoRa_wr_reg(LoRaRegister reg, uint8_t data) {
	LoRaSingleXfr result = {
		.addr = 0x80 | reg,
		.data = data
	};
	return result;
}

// Returns a 2-byte sequence to send over SPI
// Reads data from internal LoRa reg if possible
LoRaSingleXfr LoRa_rd_reg(LoRaRegister reg) {
	LoRaSingleXfr result = {
		.addr = 0x7F & reg,
		.data = 0
	};
	return result;
}

uint32_t LoRa_make_frf_bits(uint32_t mhzFrequency) {
	uint32_t result = 0;
	uint32_t frf = mhzFrequency * 16 * 1024;
	uint8_t *bytePtr = (void*)&frf; 
	uint8_t reEndian[4] = {0};

	frf &= 0x00FFFFFF;
	reEndian[0] = bytePtr[2];
	reEndian[1] = bytePtr[1];
	reEndian[2] = bytePtr[0];
	result = *(uint32_t*)reEndian; 
	return result;
}

