#include <stdio.h>
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
void* buf = &msg;

// Put the device in Sleep/LoRa mode
msg = LoRa_wr_reg(Op_Mode, 0x00);
wiringPiSPIDataRW(0, buf, 2);
msg = LoRa_wr_reg(Op_Mode, 0x80);
wiringPiSPIDataRW(0, buf, 2);

uint8_t burst[256];
uint32_t* dat = (void*)(burst + 1);
burst[0] = 0x80 | Fr_Msb;
*dat = LoRa_make_frf_bits(915);
wiringPiSPIDataRW(0, burst, 4);

// Read all reg
for (int i = 0; i < 0x80; i++) {
	printf("0x%.2x: ", i);
	msg = LoRa_rd_reg(i);
	wiringPiSPIDataRW(0, buf, 2);
	printf("0x%.4hx\n", msg);

}

}

