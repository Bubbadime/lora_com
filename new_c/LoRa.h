#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stdio.h>

#define FXOSC = 32000000.0;
#define FSTEP = (FXOSC / 524288);

typedef enum ModemConfig ModemConfig;
typedef enum LoRaRegister LoRaRegister;

typedef union LoRaSingleXfr LoRaSingleXfr;
typedef struct LoRaXfr LoRaXfr;

enum ModemConfig {
	Bw125Cr45Sf128 = (0x72 | 0x7400 | 0x040000),
	Bw500Cr45Sf128 = (0x92 | 0x7400 | 0x040000),
	Bw31_25Cr48Sf512 = (0x48 | 0x9400 | 0x040000),
	Bw125Cr48Sf4096 = (0x78 | 0xc400 | 0x0c0000)
};

enum LoRaRegister {
 Fifo = 0x00,
 Op_Mode = 0x01,
 Fr_Msb = 0x06,
 Fr_Mid = 0x07,
 Fr_Lsb = 0x08,
 Pa_Config = 0x09,
 Pa_Ramp = 0x0A,
 Ocp = 0x0B,
 Lna = 0x0C,
 Fifo_Addr_Ptr = 0x0D,
 Fifo_Tx_Base_Addr = 0x0E,
 Fifo_Rx_Base_Addr = 0x0F,
 Fifo_Rx_Current_Addr = 0x10,
 Irq_Flags_Mask = 0x11,
 Irq_Flags = 0x12,
 Rx_Nb_Bytes = 0x13,
 Rx_Header_Cnt_Value_Msb = 0x14,
 Rx_Header_Cnt_Value_Lsb = 0x15,
 Rx_Packet_Cnt_Value_Msb = 0x16,
 Rx_Packet_Cnt_Value_Lsb = 0x17,
 Modem_Stat = 0x18,
 Packet_Snr_Value = 0x19,
 Packet_Rssi_Value = 0x1A,
 Rssi_Value = 0x1B,
 Hop_Channel = 0x1C,
 Modem_Config_1 = 0x1D,
 Modem_Config_2 = 0x1E,
 Symb_Timeout_Lsb = 0x1F,
 Preamble_Msb = 0x20,
 Preamble_Lsb = 0x21,
 Payload_Length = 0x22,
 Max_Payload_Length = 0x23,
 Hop_Period = 0x24,
 Fifo_Rx_Byte_Addr = 0x25,
 Modem_Config_3 = 0x26
};

union LoRaSingleXfr {
	uint32_t fullXfr;
	struct {
		union {
			uint8_t src_base[2];
			struct {
				uint8_t addr;
				uint8_t src_data;
			};
		};
		union {
			uint8_t dst_base[2];
			struct {
				uint8_t dst_pad;
				uint8_t dst_data;
			};
		};
	};
};

struct LoRaXfr {
	size_t xfrSize;
	struct {
		uint8_t base[257];
		struct {
			uint8_t addr;
			uint8_t data[256];
		};
	};
};

void LoRa_get_reg_name(uint8_t addr, char* dst);
void LoRa_print_reg_read(LoRaSingleXfr msg);
void LoRa_print_all_reg(int fd);

LoRaSingleXfr LoRa_wr_reg(LoRaRegister reg, uint8_t data);
LoRaSingleXfr LoRa_rd_reg(LoRaRegister reg);
uint32_t LoRa_make_frf_bits(uint32_t mhzFrequency);
int32_t LoRa_xfr_single(int fd, LoRaSingleXfr *msg);
int32_t LoRa_xfr_fifo_full(int fd, uint8_t *dst);

#endif

