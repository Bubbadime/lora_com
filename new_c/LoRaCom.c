#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include <wiringPiSPI.h>

#include "LoRa.h"
#include "PlatformLoRa.h"

#pragma pack(push)
#pragma pack(1)
typedef struct PayloadHeader PayloadHeader;

typedef enum HeaderMagic1 HeaderMagic1;
typedef enum HeaderMagic2 HeaderMagic2;

struct PayloadHeader {
    union {
        uint16_t magic16;
        uint8_t magic8[2];
    };

    uint32_t callsign;
    uint8_t imageId;
    uint16_t packetId;
};

enum HeaderMagic1 {
    Sync1 = 0x88,
    Ssdv = 0x55,
    Ack1 = '!'
};
enum HeaderMagic2 {
    Sync2 = 0x00,
    Ssdv_Invalid = 0x65,
    Ssdv_Normal = 0x66,
    Ssdv_No_Fec = 0x67,
    Ack2 = '!'
};

#pragma pack(pop)
PayloadHeader dec_header(uint8_t* packet) {
    PayloadHeader result = *((PayloadHeader*)packet);
    uint8_t *id = (void*)&(result.packetId);
    uint8_t tmp;
    tmp = id[0];
    id[0] = id[1];
    id[1] = tmp;
    return result;
}
void enc_header(PayloadHeader* h) {
    uint8_t *id = (void*)&(h->packetId);
    uint8_t tmp;
    tmp = id[0];
    id[0] = id[1];
    id[1] = tmp;
    return;
}
void print_header(PayloadHeader h) {
    printf("Magic: 0x%hx, sign: 0x%x, image: %hhu, packet: %hu\n", h.magic16, h.callsign, h.imageId, h.packetId);
}

size_t get_file_size(FILE* fd) {
    size_t cur = 0;
    size_t result = 0;
    cur = ftell(fd);
    fseek(fd, 0, SEEK_END);
    result = ftell(fd);
    fseek(fd, cur, SEEK_SET);
    return result;
}

int main(int argc, char** argv) {
    // Parse opts
    int sender = 0;
    char c = 0;
    while ((c = getopt(argc, argv, "sr")) != (char)-1) {
        switch (c) {
            case 's':
                {
                    printf("Set to sender\n");
                    sender = 1;
                break;
                }
            case 'r':
                {
                    printf("Set to receiver\n");
                    sender = 0;
                break;
                }
            default:
        }

    }
    int fd = wiringPiSPISetupMode(0, 500000, SPI_MODE_0);
    // Put the device in Sleep/LoRa mode
    LoRaSingleXfr msg;
    int rtrn;
    msg = LoRa_wr_reg(Op_Mode, 0x00);
    rtrn = LoRa_xfr_single(fd, &msg);
    msg = LoRa_wr_reg(Op_Mode, 0x80);
    rtrn = LoRa_xfr_single(fd, &msg);

    // Set frequency to 915
    LoRaXfr burstMsg = {0};
    uint32_t dat = LoRa_encode_frf_bits(915);
    burstMsg = LoRa_wr_burst(Fr_Msb, (void*)&dat, 3);
    dat = LoRa_decode_frf_bits(dat);
    printf("Frf: %d\n", dat);
    LoRa_xfr_burst(fd, &burstMsg);

    // Set rx and tx base to 0
    msg = LoRa_wr_reg(Fifo_Rx_Base_Addr, 0x00);
    rtrn = LoRa_xfr_single(fd, &msg);
    msg = LoRa_wr_reg(Fifo_Tx_Base_Addr, 0x00);
    rtrn = LoRa_xfr_single(fd, &msg);

    // Set payload length
    msg = LoRa_wr_reg(Payload_Length, 0xFF);
    rtrn = LoRa_xfr_single(fd, &msg);

    LoRaXfr pack;
    if (sender) {
        uint8_t* buf;
        size_t size;
        FILE* ssdvFd = fopen("out.ssdv", "rb");
        size = get_file_size(ssdvFd);
        buf = malloc(size);
        fread(buf, 1, size, ssdvFd);
        for (size_t i = 0; i < size; i += 255) {
            // Reset Fifo addr ptr
            msg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
            rtrn = LoRa_xfr_single(fd, &msg);

            // Write the packet into the Fifo
            pack = LoRa_wr_fifo_bytes(buf + i, 255);
            PayloadHeader h = dec_header(pack.src_data);
            print_header(h);
            LoRa_xfr_fifo_bytes(fd, 255, &pack);

            // Set mode to tx and wait for 'done' irq
            msg = LoRa_wr_reg(Op_Mode, 0x83);
            rtrn = LoRa_xfr_single(fd, &msg);
            LoRa_wait_irq_all(fd, LoRa_Irq_Tx_Done_Bit);
        }
        fclose(ssdvFd);

        char *eomBuf = "EOM";
        msg = LoRa_wr_reg(Payload_Length, 0x04);
        rtrn = LoRa_xfr_single(fd, &msg);
        // Reset Fifo addr ptr
        msg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
        rtrn = LoRa_xfr_single(fd, &msg);

        // Write the packet into the Fifo
        pack = LoRa_wr_fifo_bytes((void*)eomBuf, 4);
        LoRa_xfr_fifo_bytes(fd, 0x00, &pack);

        // Set mode to tx and wait for 'done' irq
        msg = LoRa_wr_reg(Op_Mode, 0x83);
        rtrn = LoRa_xfr_single(fd, &msg);
        LoRa_wait_irq_all(fd, LoRa_Irq_Tx_Done_Bit);

    }

    else {
        FILE* ssdvFd = fopen("out.ssdv", "wb");
        msg = LoRa_wr_reg(Op_Mode, 0x80 | LoRa_Op_Mode_Rx_Continuous);
        rtrn = LoRa_xfr_single(fd, &msg);
        uint8_t eom = 0;
        while (!eom) {

            // Keep trying to rx until success
            uint32_t irqId = 0;
            while (irqId == 0) {
                irqId = LoRa_wait_irq(fd, LoRa_Irq_Rx_Done_Bit | LoRa_Irq_Valid_Header_Bit, 1);
            }

            // Read the number of bytes received
            msg = LoRa_rd_reg(Rx_Nb_Bytes);
            LoRa_xfr_single(fd, &msg);
            uint8_t rxNbBytes = msg.dst_data;

            // Set fifo addr ptr to pick up the packet
            msg = LoRa_rd_reg(Fifo_Rx_Current_Addr);
            LoRa_xfr_single(fd, &msg);
            uint8_t rxAddr = msg.dst_data;

            // Reset Fifo addr ptr
            msg = LoRa_wr_reg(Fifo_Addr_Ptr, rxAddr);
            rtrn = LoRa_xfr_single(fd, &msg);

            printf("%hhu Bytes rx, ", rxNbBytes);
            // Read packet from fifo
            pack = LoRa_rd_fifo_bytes((size_t)rxNbBytes);;
            LoRa_xfr_fifo_bytes(fd, rxAddr, &pack);
            eom = !strncmp((void*)pack.dst_data, "EOM", 4);
            if (!eom) {
                // Extract the header info and print
                PayloadHeader h = dec_header(pack.dst_data);
                print_header(h);
                fwrite(pack.dst_data, 1, rxNbBytes, ssdvFd);
            }
        }
        fclose(ssdvFd);
    }
    // Clean up the chip on exit 

    // Reset Fifo addr ptr
    msg = LoRa_wr_reg(Fifo_Addr_Ptr, 0x00);
    rtrn = LoRa_xfr_single(fd, &msg);

    // Put in sleep mode
    msg = LoRa_wr_reg(Op_Mode, 0x80);
    rtrn = LoRa_xfr_single(fd, &msg);

    // Clear Irq
    msg = LoRa_rd_reg(Irq_Flags);
    rtrn = LoRa_xfr_single(fd, &msg);
    msg = LoRa_wr_reg(Irq_Flags, msg.dst_data);
    rtrn = LoRa_xfr_single(fd, &msg);
    return 0;
}
