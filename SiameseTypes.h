#pragma once
#include <cstdint>

#define SIAMESE_PACKET_NUM_MAX 0x3FFFFF
#define SIAMESE_MAX_RECOVERY_PACKETS 256

typedef int SiameseResult;

struct SiameseOriginalPacket {
    uint32_t PacketNum;
    void* Data;
    unsigned DataBytes;
};

struct SiameseRecoveryPacket {
    void* Data;
    unsigned DataBytes;
};

typedef void* SiameseEncoder;
typedef void* SiameseDecoder;