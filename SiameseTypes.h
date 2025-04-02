#pragma once
#include <cstdint>
#include "siamese.h" // Use definitions from siamese.h

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

typedef void* SiameseEncoder;
typedef void* SiameseDecoder;

#endif // SIAMESE_TYPES_H