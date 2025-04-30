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
config[0] = LoRa_make_config_1(500.0, 5, 0);
config[1] = LoRa_make_config_2(4096, 0, 0, 0);
config[2] = 0x00;
setConfig = LoRa_wr_burst(Modem_Config_1, config, 3);
printf("sz: %u, 0: %hhu, 1: %hhu, 2: %hhu\n", setConfig.xfrSize, setConfig.src_data[0], setConfig.src_data[1], setConfig.src_data[2]);
LoRa_xfr_burst(fd, &setConfig);

// Set mode
#if 1
msg = LoRa_wr_reg(Op_Mode, 0x81);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x84);
rtrn = LoRa_xfr_single(fd, &msg);
msg = LoRa_wr_reg(Op_Mode, 0x86);
rtrn = LoRa_xfr_single(fd, &msg);
#endif


LoRa_print_all_reg(fd);
LoRa_wait_irq_all(fd, Lora_Irq_Rx_Timeout_Bit);

LoRaXfr bigmsg = LoRa_rd_fifo_full();;
LoRa_xfr_fifo_full(fd, &bigmsg);
for (uint32_t i = 0; i < 256; i++) {
    printf("%c", bigmsg.dst_data[i]);
}
printf("\n");
return 0;

}

