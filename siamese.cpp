#include "siamese.h"
#include "SiameseEncoder.h"
#include "SiameseDecoder.h"
#include "gf256.h"

using namespace siamese;

extern "C" {

// Siamese library initialization
int siamese_init() {
    // Initialize GF256 library
    return gf256_init();
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

    unsigned usedBytes = 0; // Declare a variable for the missing argument
    unsigned maxBytes = recoveryOut->dataBytes; // Declare maxBytes based on recoveryOut->dataBytes
    uint8_t* dataBuffer = const_cast<uint8_t*>(recoveryOut->data); // Use const_cast to remove const qualifier
    return static_cast<Encoder*>(encoder)->GetRecoveryPacket(
        0, // Default recovery ID
        dataBuffer, maxBytes, usedBytes); // Pass the correct arguments
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

    // Placeholder for actual statistics gathering
    for (unsigned i = 0; i < statsCount; ++i) {
        statsOut[i] = 0;
    }

    return Siamese_Success;
}

// Decoder functions are implemented in SiameseDecoder.cpp

} // extern "C"

namespace siamese {

Encoder::Encoder() {
    // Constructor implementation
}

void Encoder::Add(SiameseOriginalPacket& packet) {
    // Add implementation
}

void Encoder::Retransmit(SiameseOriginalPacket& packet) {
    // Retransmit implementation
}

bool Encoder::GetRecoveryPacket(unsigned int id, unsigned char* buffer, unsigned int bufferSize, unsigned int& bytesWritten) {
    // GetRecoveryPacket implementation
    return true;
}

bool Encoder::Acknowledge(const unsigned char* data, unsigned int length, unsigned int& ackedBytes) {
    // Acknowledge implementation
    return true;
}

} // namespace siamese
