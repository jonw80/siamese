#ifndef SIAMESE_ENCODER_H
#define SIAMESE_ENCODER_H

#include <cstdint>
#include "SiameseResult.h" // Ensure SiameseResult is included
#include "siamese.h" // Use SiameseOriginalPacket from siamese.h

namespace siamese {

class Encoder {
public:
    Encoder();
    ~Encoder();

    SiameseResult Add(SiameseOriginalPacket& packet);
    SiameseResult Retransmit(SiameseOriginalPacket& packet);
    SiameseResult GetRecoveryPacket(unsigned id, uint8_t* buffer, unsigned bufferSize, unsigned& packetSize);
    SiameseResult Acknowledge(const uint8_t* data, unsigned length, unsigned& ackedBytes);
};

} // namespace siamese

#endif // SIAMESE_ENCODER_H
