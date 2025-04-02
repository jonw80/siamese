#ifndef ENCODER_H
#define ENCODER_H

#include "SiameseTypes.h"

namespace siamese {

class Encoder {
public:
    Encoder();
    bool Add(SiameseOriginalPacket& packet);
    bool Retransmit(SiameseOriginalPacket& packet);
    bool GetRecoveryPacket(unsigned int id, unsigned char* buffer, unsigned int bufferSize, unsigned int& packetSize);
    bool Acknowledge(const unsigned char* data, unsigned int size, unsigned int& acknowledgedCount);
};

} // namespace siamese

#endif // ENCODER_H
