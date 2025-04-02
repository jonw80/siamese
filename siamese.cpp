#include "siamese.h"
#include "SiameseEncoder.h"
#include "SiameseDecoder.h"
#include "gf256.h"

using namespace siamese;

extern "C" {

// Siamese library initialization
int siamese_init() {
    return gf256_init_(1); // Pass the required argument
}

// Encoder functions
SiameseEncoder siamese_encoder_create() {
    return new Encoder();
}

void siamese_encoder_free(SiameseEncoder encoder) {
    if (encoder) {
        delete static_cast<Encoder*>(encoder);
    }
}

int siamese_encoder_add(SiameseEncoder encoder, const SiameseOriginalPacket* packet) {
    if (!encoder || !packet) {
        return Siamese_NeedMoreData;
    }
    return static_cast<int>(static_cast<Encoder*>(encoder)->Add(const_cast<SiameseOriginalPacket&>(*packet)));
}

int siamese_encoder_retransmit(SiameseEncoder encoder, const SiameseOriginalPacket* packet) {
    if (!encoder || !packet) {
        return Siamese_NeedMoreData;
    }
    return static_cast<int>(static_cast<Encoder*>(encoder)->Retransmit(const_cast<SiameseOriginalPacket&>(*packet)));
}

int siamese_encoder_get_statistics(SiameseEncoder encoder, uint64_t* statsOut, unsigned statsCount) {
    (void)encoder; // Suppress unused parameter warning
    (void)statsOut;
    (void)statsCount;
    return Siamese_Success;
}

SiameseResult siamese_encoder_get_recovery(SiameseEncoder encoder, unsigned id, uint8_t* data, unsigned maxBytes, unsigned* usedBytesOut) {
    if (!encoder || !data || !usedBytesOut) {
        return SiameseResult::Failure;
    }
    return static_cast<Encoder*>(encoder)->GetRecoveryPacket(id, data, maxBytes, *usedBytesOut);
}

SiameseResult siamese_acknowledge(SiameseEncoder encoder, const uint8_t* data, unsigned bytes, unsigned* ackedOut) {
    if (!encoder || !data || !ackedOut) {
        return SiameseResult::Failure;
    }
    return static_cast<Encoder*>(encoder)->Acknowledge(data, bytes, *ackedOut);
}

} // extern "C"

namespace siamese {

Encoder::Encoder() {}

Encoder::~Encoder() {}

SiameseResult Encoder::Add(SiameseOriginalPacket& packet) {
    (void)packet; // Suppress unused parameter warning
    return SiameseResult::Success;
}

SiameseResult Encoder::Retransmit(SiameseOriginalPacket& packet) {
    (void)packet; // Suppress unused parameter warning
    return SiameseResult::Success;
}

SiameseResult Encoder::Acknowledge(const uint8_t* data, unsigned length, unsigned& ackedBytes) {
    (void)data; // Suppress unused parameter warning
    (void)length; // Suppress unused parameter warning
    (void)ackedBytes; // Suppress unused parameter warning
    return SiameseResult::Success;
}

SiameseResult Encoder::GetRecoveryPacket(unsigned id, uint8_t* buffer, unsigned bufferSize, unsigned& packetSize) {
    (void)id; // Suppress unused parameter warning
    (void)buffer; // Suppress unused parameter warning
    (void)bufferSize; // Suppress unused parameter warning
    (void)packetSize; // Suppress unused parameter warning
    return SiameseResult::Success;
}

} // namespace siamese
