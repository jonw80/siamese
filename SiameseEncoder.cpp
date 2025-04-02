#include "SiameseEncoder.h"
#include "SiameseResult.h" // Ensure this header is included

namespace siamese {

Encoder::Encoder() {}

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

}
