#include <stdint.h>
#include <unistd.h>;
#include <stdio.h>
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

int main(int argc, char** argv) {
    // Parse opts
    char c = 0;
    while ((c = getopt(argc, argv, "sr") != -1)) {
        printf("opt %c", c);

    }
    printf("c\n");
    FILE* fd = fopen("out2.ssdv", "rb");
    uint8_t buf[2048];
	printf("Size of header: %u\n", sizeof(PayloadHeader));
    for (uint32_t i = 0; i< 8; i++) {

    fread(buf, 1, 256, fd);
    PayloadHeader h = dec_header(buf);
    print_header(h);
    }
    return 0;
}
