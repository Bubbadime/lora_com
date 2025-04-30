#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stdio.h>


typedef enum LoRaModemConfig1Bits LoRaModemConfig1Bits;
typedef enum LoRaModemConfig2Bits LoRaModemConfig2Bits;
typedef enum LoRaModemConfig3Bits LoRaModemConfig3Bits;
typedef enum LoRaOpModeBits LoRaOpModeBits;
typedef enum LoRaRegister LoRaRegister;
typedef enum LoRaIrqBits LoRaIrqBits;

typedef union LoRaSingleXfr LoRaSingleXfr;
typedef struct LoRaXfr LoRaXfr;

typedef uint8_t LoRaModemConfig1;
typedef uint8_t LoRaModemConfig2;
typedef uint8_t LoRaModemConfig3;
typedef uint16_t LoRaModemConfig12;

// Modem Configs
enum LoRaModemConfig1Bits {
    LoRa_Config_1_Bw_7_8 = 0x00,
    LoRa_Config_1_Bw_10_4 = 0x10,
    LoRa_Config_1_Bw_15_6 = 0x20,
    LoRa_Config_1_Bw_20_8 = 0x30,
    LoRa_Config_1_Bw_31_25 = 0x40,
    LoRa_Config_1_Bw_41_7 = 0x50,
    LoRa_Config_1_Bw_62_5 = 0x60,
    LoRa_Config_1_Bw_125 = 0x70,
    LoRa_Config_1_Bw_250 = 0x80,
    LoRa_Config_1_Bw_500 = 0x90,

    LoRa_Config_1_Cr_4_5th = 0x02,
    LoRa_Config_1_Cr_4_6th = 0x04,
    LoRa_Config_1_Cr_4_7th = 0x06,
    LoRa_Config_1_Cr_4_8th = 0x08,
    LoRa_Config_1_Implicit_Header_On = 0x01,
};

enum LoRaModemConfig2Bits {
    LoRa_Config_2_Sf_64 = 0x60,
    LoRa_Config_2_Sf_128 = 0x70,
    LoRa_Config_2_Sf_256 = 0x80,
    LoRa_Config_2_Sf_512 = 0x90,
    LoRa_Config_2_Sf_1024 = 0xA0,
    LoRa_Config_2_Sf_2048 = 0xB0,
    LoRa_Config_2_Sf_4096 = 0xC0,

    LoRa_Config_2_Tx_Continuous_Mode_On = 0x08,
    LoRa_Config_2_Rx_Payload_Crc_On = 0x04,
};
// Op Modes
enum LoRaOpModeBits {
   LoRa_Op_Mode_Sleep = 0x00,
   LoRa_Op_Mode_Stdby = 0x01,
   LoRa_Op_Mode_Fstx = 0x02,
   LoRa_Op_Mode_Tx = 0x03,
   LoRa_Op_Mode_Fsrx = 0x04,
   LoRa_Op_Mode_Rx_Continuous = 0x05,
   LoRa_Op_Mode_Rx_Single = 0x06,
   LoRa_Op_Mode_Cad = 0x07,

   LoRa_Op_Mode_Long_Range = 0x80,
   LoRa_Op_Mode_Access_Shared_Reg = 0x40,
   LoRa_Op_Mode_Low_Frequency = 0x08,
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

// IRQ flag bits
enum LoRaIrqBits {
    LoRa_Irq_Cad_Detected_Bit = 0x01,
    LoRa_Irq_Fhss_Change_Channel_Bit = 0x02,
    LoRa_Irq_Cad_Done_Bit = 0x04,
    LoRa_Irq_Tx_Done_Bit = 0x08,
    LoRa_Irq_Valid_Header_Bit = 0x10,
    LoRa_Irq_Payload_Crc_Error_Bit = 0x20,
    LoRa_Irq_Rx_Done_Bit = 0x40,
    LoRa_Irq_Rx_Timeout_Bit = 0x80,
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

/* Returns config byte to write
 * bw expressed in kHz,
 * coding rate is 4/cr
 */
LoRaModemConfig1 LoRa_make_config_1(float bw, uint8_t cr, uint8_t implicitHeaderMode);

/* sf should be a power of 2 64-4096.
 * txContinuousModeOn sends packets across the Fifo, 
 * seems to be used for testing.
 */
LoRaModemConfig2 LoRa_make_config_2(uint32_t sf, uint8_t txContinuousModeOn, uint8_t rxPayloadCrcOn, uint8_t timeOutMsb);

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

