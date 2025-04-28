#include <fcntl.h>
#include <errno.h>

#include <stdint.h>
#include <stdio.h>

#include <asm/ioctl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

int32_t spi_xfr(int fd, size_t size, void* src, void* dst) {
	int32_t result = -1;
	struct spi_ioc_transfer msg = {0};
	
	msg.tx_buf = (uint32_t)src;
	msg.rx_buf = (uint32_t)dst;
	msg.len = size;
	
	result = ioctl(fd, SPI_IOC_MESSAGE(1), &msg);
	return result;
}

