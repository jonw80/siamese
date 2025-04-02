#ifndef SIAMESE_ENCODER_H
#define SIAMESE_ENCODER_H

#include <cstdint>
#include "SiameseResult.h" // Ensure SiameseResult is included
#include "siamese.h" // Use SiameseOriginalPacket from siamese.h

namespace siamese {

class Encoder {
public:
    Encoder();
    virtual ~Encoder(); // Add virtual destructor
    SiameseResult Add(SiameseOriginalPacket& packet);
    SiameseResult Retransmit(SiameseOriginalPacket& packet);
    SiameseResult Acknowledge(const uint8_t* data, unsigned length, unsigned& ackedBytes);
    SiameseResult GetRecoveryPacket(unsigned id, uint8_t* buffer, unsigned bufferSize, unsigned& packetSize);
};

} // namespace siamese

#endif // SIAMESE_ENCODER_H
