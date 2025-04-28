// Cstd headers
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Non Cstd headers
#include <wiringPi.h>
#include <wiringPiSPI.h>

// Custom Headers
#include "LoRa.h"
#include "PlatformLoRa.h"

void LoRa_get_reg_name(uint8_t addr, char* dst) {
	static const char* LoRaRegName[0x27] = {0}; 
	static int init = 0;
	if (!init) {
	 LoRaRegName[0x00] = "Fifo";
	 LoRaRegName[0x01] = "Op_Mode";
	 LoRaRegName[0x02] = "reserved";
	 LoRaRegName[0x03] = "reserved";
	 LoRaRegName[0x04] = "reserved";
	 LoRaRegName[0x05] = "reserved";
	 LoRaRegName[0x06] = "Fr_Msb";
	 LoRaRegName[0x07] = "Fr_Mid";
	 LoRaRegName[0x08] = "Fr_Lsb";
	 LoRaRegName[0x09] = "Pa_Config";
	 LoRaRegName[0x0A] = "Pa_Ramp";
	 LoRaRegName[0x0B] = "Ocp";
	 LoRaRegName[0x0C] = "Lna";
	 LoRaRegName[0x0D] = "Fifo_Addr_Ptr";
	 LoRaRegName[0x0E] = "Fifo_Tx_Base_Addr";
	 LoRaRegName[0x0F] = "Fifo_Rx_Base_Addr";
	 LoRaRegName[0x10] = "Fifo_Rx_Current_Addr";
	 LoRaRegName[0x11] = "Irq_Flags_Mask";
	 LoRaRegName[0x12] = "Irq_Flags";
	 LoRaRegName[0x13] = "Rx_Nb_Bytes";
	 LoRaRegName[0x14] = "Rx_Header_Cnt_Value_Msb";
	 LoRaRegName[0x15] = "Rx_Header_Cnt_Value_Lsb";
	 LoRaRegName[0x16] = "Rx_Packet_Cnt_Value_Msb";
	 LoRaRegName[0x17] = "Rx_Packet_Cnt_Value_Lsb";
	 LoRaRegName[0x18] = "Modem_Stat";
	 LoRaRegName[0x19] = "Packet_Snr_Value";
	 LoRaRegName[0x1A] = "Packet_Rssi_Value";
	 LoRaRegName[0x1B] = "Rssi_Value";
	 LoRaRegName[0x1C] = "Hop_Channel";
	 LoRaRegName[0x1D] = "Modem_Config_1";
	 LoRaRegName[0x1E] = "Modem_Config_2";
	 LoRaRegName[0x1F] = "Symb_Timeout_Lsb";
	 LoRaRegName[0x20] = "Preamble_Msb";
	 LoRaRegName[0x21] = "Preamble_Lsb";
	 LoRaRegName[0x22] = "Payload_Length";
	 LoRaRegName[0x23] = "Max_Payload_Length";
	 LoRaRegName[0x24] = "Hop_Period";
	 LoRaRegName[0x25] = "Fifo_Rx_Byte_Addr";
	 LoRaRegName[0x26] = "Modem_Config_3";
	 init = 1;
	}
	strcpy(dst, LoRaRegName[addr & 0x7f]);
	return;	
}

void LoRa_print_reg_read(LoRaSingleXfr msg) {
		char name[32] = {0};
		LoRa_get_reg_name(msg.addr, name);
		printf("0x%.2x %-23s: ", msg.addr, name);
		printf("0x%.2hhx\n", msg.dst_data);
}

// Read all reg
void LoRa_print_all_reg(int fd) {
	for (int i = 1; i < 0x27; i++) {
		LoRaSingleXfr msg = LoRa_rd_reg(i);
		LoRa_xfr_single(fd, &msg);
		LoRa_print_reg_read(msg);

	}
	return;
}
// Returns a 2-byte sequence to send over SPI
// Writes data to internal LoRa reg if possible
LoRaSingleXfr LoRa_wr_reg(LoRaRegister reg, uint8_t data) {
	LoRaSingleXfr result = {
		.addr = 0x80 | reg,
		.src_data = data,
		.dst_pad = 0,
		.dst_data = 0
	};
	return result;
}

// Returns a 2-byte sequence to send over SPI
// Reads data from internal LoRa reg if possible
LoRaSingleXfr LoRa_rd_reg(LoRaRegister reg) {
	LoRaSingleXfr result = {
		.addr = 0x7F & reg,
		.src_data = 0,
		.dst_pad = 0,
		.dst_data = 0
	};
	return result;
}

LoRaXfr LoRa_wr_burst(LoRaRegister startReg, uint8_t* data, size_t length) {
	LoRaXfr result = {
        .xfrSize = length,
        .addr = 0x80 | startReg
	};
    if (data) {
        for (size_t i = 0; i < length; i++) {
            result.src_data[i] = data[i];
        }
    }
	return result;
}

LoRaXfr LoRa_rd_burst(LoRaRegister startReg, size_t length) {
	LoRaXfr result = {
        .xfrSize = length,
		.addr = 0x7F & startReg
	};
	return result;
}
LoRaXfr LoRa_wr_fifo_full(uint8_t *data) {
    LoRaXfr result = {0};
    result.addr = 0x80 | Fifo;
    if (data) {
        for (uint32_t i = 0; i < 256; i++) {
            result.src_data[i] = data[i];
        }
    }
    result.xfrSize = 257;
    return result;

}

LoRaXfr LoRa_rd_fifo_full() {
    LoRaXfr result = {0};
    result.addr = Fifo;
    result.xfrSize = 257;
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

// Converts frf bits into mhz frequency
uint32_t LoRa_translate_frf_bits(uint32_t frfBits) {
	uint32_t result = 0;
	uint8_t *bytePtr = (void*)&frfBits; 
	uint8_t reEndian[4] = {0};

	reEndian[0] = bytePtr[2];
	reEndian[1] = bytePtr[1];
	reEndian[2] = bytePtr[0];
    frfBits = *((uint32_t*)reEndian);
	uint32_t mhzFrequency = frfBits / 16 / 1024;
	result = *(uint32_t*)reEndian; 
	return result;

}

int32_t LoRa_xfr_burst(int fd, LoRaXfr *msg) {
	int32_t result = 0;
	result = spi_xfr(fd, msg->xfrSize, msg->src_base, msg->dst_base);
	return result;
}

int32_t LoRa_xfr_single(int fd, LoRaSingleXfr *msg) {
	int32_t result = 0;
	result = spi_xfr(fd, 2, msg->src_base, msg->dst_base);
	return result;
}

int32_t LoRa_xfr_fifo_full(int fd, LoRaXfr *msg) {
	int32_t result = 0;
	// Set fifo ptr to 0x00
	LoRaSingleXfr ptrResetMsg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
	LoRa_xfr_single(fd, &ptrResetMsg);
	result = spi_xfr(fd, msg->xfrSize, msg->src_base, msg->dst_base);
	return result;
}

