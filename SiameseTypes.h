#pragma once
#include <cstdint>

#ifndef SIAMESE_TYPES_H
#define SIAMESE_TYPES_H

#ifndef SIAMESE_PACKET_NUM_MAX
#define SIAMESE_PACKET_NUM_MAX 0x3FFFFF
#endif

#define SIAMESE_MAX_RECOVERY_PACKETS 256

typedef int SiameseResult;

struct SiameseOriginalPacket {
    const uint8_t* data;
    unsigned dataBytes;
    unsigned packetNum;
};

struct SiameseRecoveryPacket {
    const uint8_t* data;
    unsigned dataBytes;
};

typedef void* SiameseEncoder;
typedef void* SiameseDecoder;

#endif // SIAMESE_TYPES_H