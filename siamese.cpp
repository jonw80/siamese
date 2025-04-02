#include "siamese.h"
#include "SiameseEncoder.h"
#include "SiameseDecoder.h"
#include "gf256.h"

using namespace siamese;

extern "C" {

// Siamese library initialization
int siamese_init() {
    return gf256_init_(); // Ensure the correct GF256 initialization function is called
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

SiameseResult siamese_encoder_add(SiameseEncoder encoder, const SiameseOriginalPacket* packet) {
    if (!encoder || !packet) {
        return Siamese_NeedMoreData;
    }

    return static_cast<Encoder*>(encoder)->Add(const_cast<SiameseOriginalPacket&>(*packet));
}

SiameseResult siamese_encoder_retransmit(SiameseEncoder encoder, const SiameseOriginalPacket* packet) {
    if (!encoder || !packet) {
        return Siamese_NeedMoreData;
    }

    return static_cast<Encoder*>(encoder)->Retransmit(const_cast<SiameseOriginalPacket&>(*packet));
}

SiameseResult siamese_encode(SiameseEncoder encoder, SiameseRecoveryPacket* recoveryOut) {
    if (!encoder || !recoveryOut) {
        return Siamese_NeedMoreData;
    }

    unsigned usedBytes = 0;
    unsigned maxBytes = recoveryOut->dataBytes;
    uint8_t* dataBuffer = const_cast<uint8_t*>(recoveryOut->data);
    return static_cast<Encoder*>(encoder)->GetRecoveryPacket(0, dataBuffer, maxBytes, usedBytes);
}

SiameseResult siamese_encoder_get_recovery(SiameseEncoder encoder, unsigned id, uint8_t* data, unsigned maxBytes, unsigned* usedBytesOut) {
    if (!encoder || !data || !usedBytesOut) {
        return Siamese_NeedMoreData;
    }

    return static_cast<Encoder*>(encoder)->GetRecoveryPacket(id, data, maxBytes, *usedBytesOut);
}

SiameseResult siamese_acknowledge(SiameseEncoder encoder, const uint8_t* data, unsigned bytes, unsigned* ackedOut) {
    if (!encoder || !data || !ackedOut) {
        return Siamese_NeedMoreData;
    }

    return static_cast<Encoder*>(encoder)->Acknowledge(data, bytes, *ackedOut);
}

SiameseResult siamese_encoder_get_statistics(SiameseEncoder encoder, uint64_t* statsOut, unsigned statsCount) {
    if (!encoder || !statsOut) {
        return Siamese_NeedMoreData;
    }

    for (unsigned i = 0; i < statsCount; ++i) {
        statsOut[i] = 0;
    }

    return Siamese_Success;
}

} // extern "C"

namespace siamese {

Encoder::Encoder() {
    // Constructor implementation
}

Encoder::~Encoder() {
    // Destructor implementation
}

SiameseResult Encoder::Add(SiameseOriginalPacket& packet) {
    // Add implementation
    return Siamese_Success;
}

SiameseResult Encoder::Retransmit(SiameseOriginalPacket& packet) {
    // Retransmit implementation
    return Siamese_Success;
}

SiameseResult Encoder::GetRecoveryPacket(unsigned id, uint8_t* data, unsigned maxBytes, unsigned& usedBytes) {
    // GetRecoveryPacket implementation
    usedBytes = 0; // Example: Set usedBytes to 0
    return Siamese_Success;
}

SiameseResult Encoder::Acknowledge(const uint8_t* data, unsigned bytes, unsigned& outAcked) {
    // Acknowledge implementation
    outAcked = 0; // Example: Set outAcked to 0
    return Siamese_Success;
}

} // namespace siamese
