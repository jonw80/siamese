#ifndef SIAMESE_ENCODER_H
#define SIAMESE_ENCODER_H

#include <cstdint>

namespace siamese {

struct SiameseOriginalPacket {
    // Define the structure as needed
};

enum SiameseResult {
    Siamese_Success,
    Siamese_NeedMoreData,
    // Add other result codes as needed
};

class Encoder {
public:
    Encoder();
    ~Encoder();

    SiameseResult Add(SiameseOriginalPacket& packet);
    SiameseResult Retransmit(SiameseOriginalPacket& packet);
    SiameseResult GetRecoveryPacket(unsigned id, uint8_t* data, unsigned maxBytes, unsigned& usedBytes);
    SiameseResult Acknowledge(const uint8_t* data, unsigned bytes, unsigned& outAcked);
};

} // namespace siamese

#endif // SIAMESE_ENCODER_H
