#include <stdint.h>
#include <stdio.h>
#include "LoRa.h"
#include "PlatformLoRa.h"

#pragma pack(push)
#pragma pack(1)
typedef struct PayloadHeader PayloadHeader;
struct PayloadHeader {
    union {
        uint16_t magic16;
        uint8_t magic8[2];
    };

    uint32_t callsign;
    uint8_t imageId;
    uint16_t packetId;
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
void print_header(PayloadHeader header) {
    printf("Magic: 0x%hx, sign: 0x%x, image: %hhu, packet: %hu\n", header.magic16, header.callsign, header.imageId, header.packetId);
}
uint32_t main() {
    FILE* fd = fopen("out2.ssdv", "rb");
    uint8_t buf[2048];
	printf("Size of header: %p\n", sizeof(PayloadHeader));
for (uint32_t i = 0; i< 8; i++) {

    fread(buf, 1, 256, fd);
    PayloadHeader h = dec_header(buf);
    print_header(h);
}
    return 0;
}
