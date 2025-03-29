#pragma once
#include "SiameseTypes.h"

namespace siamese {

class Encoder {
public:
    Encoder();

    SiameseResult Add(SiameseOriginalPacket& packet);
    SiameseResult Retransmit(SiameseOriginalPacket& packet);
    SiameseResult Acknowledge(const uint8_t* data, unsigned bytes, unsigned& outAcked);
    SiameseResult GetRecoveryPacket(unsigned id, uint8_t* data, unsigned maxBytes, unsigned& usedBytes);
};

} // namespace siamese
