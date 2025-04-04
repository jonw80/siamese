#ifndef SIAMESE_ENCODER_H
#define SIAMESE_ENCODER_H

#include "siamese.h"
#include <vector>
#include <cstdint>

namespace siamese {

class Encoder {
public:
    Encoder();

    // Add a packet to the encoder's working set
    bool Add(SiameseOriginalPacket& packet);

    // Re-add a packet that may have already been sent
    bool Retransmit(SiameseOriginalPacket& packet);

    // Generate a FEC (recovery) packet
    bool GetRecoveryPacket(unsigned int id, unsigned char* buffer, unsigned int bufferSize, unsigned int& packetSize);

    // Inform encoder of which packets have been received by the decoder
    bool Acknowledge(const unsigned char* data, unsigned int size, unsigned int& acknowledgedCount);

private:
    struct StoredPacket {
        std::vector<uint8_t> data;
        unsigned int packetNum;
    };

    std::vector<StoredPacket> packets;
};

} // namespace siamese

#endif // SIAMESE_ENCODER_H
