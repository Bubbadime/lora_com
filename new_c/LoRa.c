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
	result = frfBits / 16 / 1024;
	return result;

}

LoRaModemConfig1 LoRa_make_config_1(float bw, uint8_t cr, uint8_t implicitHeaderMode) {
    LoRaModemConfig1 result = 0;
    if (bw < (10.4 + 7.8) / 2.0) {
        result |= LoRa_Config_1_Bw_7_8;
    }
    else if (bw < (15.6 + 10.4) / 2.0) {
        result |= LoRa_Config_1_Bw_10_4;
    }
    else if (bw < (20.8 + 15.6) / 2.0) {
        result |= LoRa_Config_1_Bw_15_6;
    }
    else if (bw < (31.25 + 20.8) / 2.0) {
        result |= LoRa_Config_1_Bw_20_8;
    }
    else if (bw < (41.7 + 31.25) / 2.0) {
        result |= LoRa_Config_1_Bw_31_25;
    }
    else if (bw < (62.5 + 41.7) / 2.0) {
        result |= LoRa_Config_1_Bw_41_7;
    }
    else if (bw < (125.0 + 62.5) / 2.0) {
        result |= LoRa_Config_1_Bw_62_5;
    }
    else if (bw < (250.0 + 125.0) / 2.0) {
        result |= LoRa_Config_1_Bw_125;
    }
    else if (bw < (500.0 + 250.0) / 2.0) {
        result |= LoRa_Config_1_Bw_250;
    }
    else {
        result |= LoRa_Config_1_Bw_500;
    }
    switch (cr) {
        case 5:
            {
                result |= LoRa_Config_1_Cr_4_5th;
                break;
            } 
        case 6:
            {
                result |= LoRa_Config_1_Cr_4_6th;
                break;
            } 
        case 7:
            {
                result |= LoRa_Config_1_Cr_4_7th;
                break;
            } 
        case 8:
            {
                result |= LoRa_Config_1_Cr_4_8th;
                break;
            } 
        default: 
            {
                result |= LoRa_Config_1_Cr_4_5th;
            }
    }
    if (implicitHeaderMode) {
        result |= LoRa_Config_1_Implicit_Header_On;
    }
    return result;
}

LoRaModemConfig2 LoRa_make_config_2(uint32_t sf, uint8_t txContinuousModeOn, uint8_t rxPayloadCrcOn, uint8_t timeOutMsb);

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

// Waits for any one interrupt from irqBits to be set
uint8_t LoRa_wait_irq_any(int fd, uint8_t irqBits) {
    uint8_t result = 0;
    uint8_t irqMask;
    LoRaSingleXfr msg = LoRa_rd_reg(Irq_Flags_Mask);
    LoRa_xfr_single(fd, &msg);
    irqMask = (msg.dst_data & irqBits);
    if (irqMask == irqBits) {
        return result;
    }
    do {
        msg = LoRa_rd_reg(Irq_Flags);
        LoRa_xfr_single(fd, &msg);
        result = msg.dst_data & irqBits;
    } while(result  == 0);
    msg = LoRa_wr_reg(Irq_Flags, result);
    LoRa_xfr_single(fd, &msg);
    return result;
}

// Waits for all interrupts from irqBits to be set
uint8_t LoRa_wait_irq_all(int fd, uint8_t irqBits) {
    uint8_t result = 0;
    uint8_t irqMask;
    LoRaSingleXfr msg = LoRa_rd_reg(Irq_Flags_Mask);
    LoRa_xfr_single(fd, &msg);
    irqMask = (msg.dst_data & irqBits);
    if (irqMask == irqBits) {
        return result;
    }
    do {
        msg = LoRa_rd_reg(Irq_Flags);
        LoRa_xfr_single(fd, &msg);
        result = msg.dst_data & irqBits;
    } while(result  != irqBits);
    msg = LoRa_wr_reg(Irq_Flags, result);
    LoRa_xfr_single(fd, &msg);
    return result;
}
