#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stdio.h>

// Op Modes
#define LORA_OP_MODE_SLEEP_BITS 0x00
#define LORA_OP_MODE_STDBY_BITS 0x01
#define LORA_OP_MODE_FSTX_BITS 0x02
#define LORA_OP_MODE_TX_BITS 0x03
#define LORA_OP_MODE_FSRX_BITS 0x04
#define LORA_OP_MODE_RX_CONTINUOUS_BITS 0x05
#define LORA_OP_MODE_RX_SINGLE_BITS 0x06
#define LORA_OP_MODE_CAD_BITS 0x07

#define LORA_OP_MODE_LONG_RANGE_BIT 0x80
#define LORA_OP_MODE_ACCESS_SHARED_REG_BIT 0x40
#define LORA_OP_MODE_LOW_FREQUENCY_BIT 0x08


// IRQ flag bits
#define LORA_IRQ_CAD_DETECTED_BIT 0x01
#define LORA_IRQ_FHSS_CHANGE_CHANNEL_BIT 0x02
#define LORA_IRQ_CAD_DONE_BIT 0x04
#define LORA_IRQ_TX_DONE_BIT 0x08
#define LORA_IRQ_VALID_HEADER_BIT 0x10
#define LORA_IRQ_PAYLOAD_CRC_ERROR_BIT 0x20
#define LORA_IRQ_RX_DONE_BIT 0x40
#define LORA_IRQ_RX_TIMEOUT_BIT 0x80


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
    union {
        uint8_t src_base[257];
        struct {
            uint8_t addr;
            uint8_t src_data[256];
        };
    };
    union {
        uint8_t dst_base[257];
        struct {
            uint8_t dst_pad;
            uint8_t dst_data[256];
        };
    };
};

void LoRa_get_reg_name(uint8_t addr, char* dst);
void LoRa_print_reg_read(LoRaSingleXfr msg);
void LoRa_print_all_reg(int fd);
uint32_t LoRa_make_frf_bits(uint32_t mhzFrequency);
uint32_t LoRa_translate_frf_bits(uint32_t frfBits);

int32_t LoRa_xfr_burst(int fd, LoRaXfr *msg);
int32_t LoRa_xfr_single(int fd, LoRaSingleXfr *msg);
int32_t LoRa_xfr_fifo_full(int fd, LoRaXfr *dst);

LoRaXfr LoRa_wr_burst(LoRaRegister startReg, uint8_t* data, size_t length);
LoRaXfr LoRa_rd_burst(LoRaRegister startReg, size_t length);
LoRaSingleXfr LoRa_wr_reg(LoRaRegister reg, uint8_t data);
LoRaSingleXfr LoRa_rd_reg(LoRaRegister reg);
LoRaXfr LoRa_wr_fifo_full(uint8_t *src);
LoRaXfr LoRa_rd_fifo_full();

uint8_t LoRa_wait_irq_any(int fd, uint8_t irqBits);
uint8_t LoRa_wait_irq_all(int fd, uint8_t irqBits);

#endif

