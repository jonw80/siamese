#ifndef SIAMESE_H
#define SIAMESE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>  // For standard types like uint64_t
#include "SiameseResult.h" // Include SiameseResult

typedef struct SiameseOriginalPacket {
    const uint8_t* data;
    unsigned dataBytes;
    unsigned packetNum;
} SiameseOriginalPacket;

typedef struct SiameseRecoveryPacket {
    const uint8_t* data;
    unsigned dataBytes;
} SiameseRecoveryPacket;

// Opaque encoder/decoder handles
typedef void* SiameseEncoder;
typedef void* SiameseDecoder;

// Status codes
#define Siamese_Success 0
#define Siamese_NeedMoreData 1
#define Siamese_Disabled 2
#define Siamese_DuplicateData 3

// Packet macros
#define SIAMESE_PACKET_NUM_COUNT (1 << 22)
#define SIAMESE_PACKET_NUM_INC(x) (((x) + 1) % SIAMESE_PACKET_NUM_COUNT)
#define SIAMESE_MAX_PACKETS 256
#define SIAMESE_PACKET_NUM_MAX 65535

// API declarations
int siamese_encoder_add(SiameseEncoder encoder, const SiameseOriginalPacket* packet);
int siamese_encoder_retransmit(SiameseEncoder encoder, const SiameseOriginalPacket* packet);
int siamese_encoder_get_statistics(SiameseEncoder encoder, uint64_t* statsOut, unsigned statsCount);
siamese::SiameseResult siamese_encoder_get_recovery(SiameseEncoder encoder, unsigned id, uint8_t* data, unsigned maxBytes, unsigned* usedBytesOut); // Qualified return type
void siamese_encoder_free(SiameseEncoder encoder);

int siamese_decoder_add_original(SiameseDecoder decoder, const SiameseOriginalPacket* packet);
int siamese_decoder_add_recovery(SiameseDecoder decoder, const SiameseRecoveryPacket* packet);
int siamese_decoder_get(SiameseDecoder decoder, SiameseOriginalPacket* packetOut);
int siamese_decoder_decode(SiameseDecoder decoder, SiameseOriginalPacket** packetsOut, unsigned* packetCountOut);
int siamese_decoder_get_statistics(SiameseDecoder decoder, uint64_t* statsOut, unsigned statsCount);
void siamese_decoder_free(SiameseDecoder decoder);

// Optional API to create encoder/decoder instances
SiameseEncoder siamese_encoder_create();
SiameseDecoder siamese_decoder_create();

#ifdef __cplusplus
}
#endif

#endif // SIAMESE_H

