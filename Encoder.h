#pragma once
#include <vector>
#include <cstdint>

namespace siamese {

struct SiameseOriginalPacket {
    const uint8_t* Data;
    unsigned DataBytes;
    unsigned PacketNum;
};

struct OriginalPacket {
    std::vector<uint8_t> Data;
    unsigned DataBytes;
    unsigned PacketNum;
};

class Encoder {
public:
    Encoder();

    bool Add(SiameseOriginalPacket& packet);
    bool Retransmit(SiameseOriginalPacket& packet);
    bool GetRecoveryPacket(unsigned id, unsigned char* buffer, unsigned bufferSize, unsigned& packetSize);
    bool Acknowledge(const unsigned char* data, unsigned size, unsigned& acknowledgedCount);

private:
    std::vector<OriginalPacket> originals;
    unsigned packet_count;
};

} // namespace siamese
