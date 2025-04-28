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
uint8_t burst[256];
uint32_t* dat = (void*)(burst + 1);
burst[0] = 0x80 | Fr_Msb;
*dat = LoRa_make_frf_bits(915);
wiringPiSPIDataRW(0, burst, 4);

// Reset the Fifo addr
msg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Reset the Fifo addr
msg = LoRa_wr_reg(Modem_Config_3, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Unmask all interrupts 
msg = LoRa_wr_reg(Irq_Flags_Mask, 0x00);
rtrn = LoRa_xfr_single(fd, &msg);

// Set mode
#if 1
msg = LoRa_wr_reg(Op_Mode, 0x81);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x84);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x85);
rtrn = LoRa_xfr_single(fd, &msg);
#endif

LoRa_print_all_reg(fd);

uint8_t ldat[256];
for (uint32_t i = 0; i < 256; i++) {
    ldat[i] = 'L';
}
LoRaXfr bigmsg = LoRa_wr_fifo_full(ldat);;
LoRa_xfr_fifo_full(fd, &bigmsg);

bigmsg = LoRa_rd_fifo_full();;
LoRa_xfr_fifo_full(fd, &bigmsg);
for (uint32_t i = 0; i < 256; i++) {
    printf("%c", bigmsg.dst_data[i]);
}
printf("\n");
return 0;

}

