#ifndef SIAMESE_H
#define SIAMESE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Forward declarations of packet structures
struct SiameseOriginalPacket;
struct SiameseRecoveryPacket;

// Handle types
typedef void* SiameseEncoder;
typedef void* SiameseDecoder;

// Status codes
#define Siamese_Success 0
#define Siamese_NeedMoreData 1
#define Siamese_Disabled 2
#define Siamese_DuplicateData 3

// Packet number macros
#define SIAMESE_PACKET_NUM_COUNT (1 << 22)
#define SIAMESE_PACKET_NUM_INC(x) (((x) + 1) % SIAMESE_PACKET_NUM_COUNT)

// API declarations
int siamese_encoder_add(SiameseEncoder encoder, const struct SiameseOriginalPacket* packet);
int siamese_encoder_retransmit(SiameseEncoder encoder, const struct SiameseOriginalPacket* packet);
int siamese_encoder_get_statistics(SiameseEncoder encoder, uint64_t* statsOut, unsigned statsCount);

int siamese_decoder_add_original(SiameseDecoder decoder, const struct SiameseOriginalPacket* packet);
int siamese_decoder_add_recovery(SiameseDecoder decoder, const struct SiameseRecoveryPacket* packet);
int siamese_decoder_get(SiameseDecoder decoder, struct SiameseOriginalPacket* packetOut);
int siamese_decoder_decode(SiameseDecoder decoder, struct SiameseOriginalPacket** packetsOut, unsigned* packetCountOut);
int siamese_decoder_get_statistics(SiameseDecoder decoder, uint64_t* statsOut, unsigned statsCount);

#ifdef __cplusplus
}
#endif

#endif // SIAMESE_H
