#pragma once
#include <cstdint>

#ifndef SIAMESE_TYPES_H
#define SIAMESE_TYPES_H

#ifndef SIAMESE_PACKET_NUM_MAX
#define SIAMESE_PACKET_NUM_MAX 0x3FFFFF
#endif

#ifndef SIAMESE_MAX_PACKETS
#define SIAMESE_MAX_PACKETS 1024
#endif

#define SIAMESE_MAX_RECOVERY_PACKETS 256

typedef int SiameseResult;

#ifndef SIAMESE_ORIGINAL_PACKET_DEFINED
#define SIAMESE_ORIGINAL_PACKET_DEFINED
struct SiameseOriginalPacket {
    const uint8_t* data;
    unsigned dataBytes;
    unsigned packetNum;
};
#endif

#ifndef SIAMESE_RECOVERY_PACKET_DEFINED
#define SIAMESE_RECOVERY_PACKET_DEFINED
struct SiameseRecoveryPacket {
    const uint8_t* data;
    unsigned dataBytes;
};
#endif

typedef void* SiameseEncoder;
typedef void* SiameseDecoder;

#endif // SIAMESE_TYPES_H