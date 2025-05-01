#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include "LoRa.h"

int main() {
// Open LoRa device
int fd = wiringPiSPISetupMode(0, 500000, SPI_MODE_0);
int mode = 0;
ioctl(fd, SPI_IOC_RD_MODE, &mode);
printf("mode = %d\n", mode);
LoRaSingleXfr msg;
printf("fd: %x\n", fd);

int rtrn;

// Put the device in Sleep/LoRa mode
msg = LoRa_wr_reg(Op_Mode, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x80);
rtrn = LoRa_xfr_single(fd, &msg);
printf("0x%.4hx, rtrn: %d\n", msg.fullXfr, rtrn);

// Set frequency to 915
LoRaXfr burstMsg = {0};
uint32_t dat = LoRa_make_frf_bits(915);
burstMsg = LoRa_wr_burst(Fr_Msb, (void*)&dat, 3);
dat = LoRa_translate_frf_bits(dat);
printf("Frf: %d\n", dat);
LoRa_xfr_burst(fd, &burstMsg);

// Reset the Fifo addr
msg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Reset the Fifo addr
msg = LoRa_wr_reg(Modem_Config_3, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Unmask all interrupts 
msg = LoRa_wr_reg(Irq_Flags_Mask, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Set config and timeout
LoRaXfr setConfig;
uint8_t config[3] = {0};
config[0] = LoRa_make_config_1(125.0, 5, 0);
config[1] = LoRa_make_config_2(128, 0, 0, 3);
config[2] = 0xFF;
setConfig = LoRa_wr_burst(Modem_Config_1, config, 3);
LoRa_xfr_burst(fd, &setConfig);

config[0] = 0x00;
config[1] = 0x80;
setConfig = LoRa_wr_burst(Preamble_Msb, config, 2);
LoRa_xfr_burst(fd, &setConfig);

// Set mode
#if 1
msg = LoRa_wr_reg(Op_Mode, 0x81);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x84);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x80 | LoRa_Op_Mode_Rx_Continuous);
rtrn = LoRa_xfr_single(fd, &msg);
#endif


LoRa_print_all_reg(fd);

uint8_t last = -1;
for (uint32_t i = 0; i < 64; i++) {
    // Rx a packet
    uint32_t irqId = 0;
    while (irqId == 0) {
        irqId = LoRa_wait_irq(fd, LoRa_Irq_Rx_Done_Bit | LoRa_Irq_Valid_Header_Bit, 1);
    }

    msg = LoRa_rd_reg(Rx_Nb_Bytes);
    LoRa_xfr_single(fd, &msg);
    uint8_t rxNbBytes = msg.dst_data;
    printf("%hhu Bytes rx\n", rxNbBytes);

    msg = LoRa_rd_reg(Fifo_Rx_Current_Addr);
    LoRa_xfr_single(fd, &msg);
    uint8_t rxAddr = msg.dst_data;
    printf("%hhu rx ptr in fifo\n", rxAddr);

    LoRaXfr bigmsg = LoRa_rd_fifo_bytes((size_t)rxNbBytes);;
    LoRa_xfr_fifo_bytes(fd, rxAddr, &bigmsg);
    if (last != bigmsg.dst_data[2]) {
        for (uint32_t i = 0; i < rxNbBytes; i++) {
            if ( i < 4) {
                printf("%hhu ", bigmsg.dst_data[i]);
            }
            else {
                printf("%c", bigmsg.dst_data[i]);
            }
        }
        last != bigmsg.dst_data[2]; 
    }
    printf("\n");

}
// return
return 0;
}

