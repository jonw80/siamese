#include "Encoder.h"
#include "gf256.h"
#include <cstring>

namespace siamese {

Encoder::Encoder()
    : nextPacketNum(0)
{
    gf256_init();  // Ensure GF256 context is initialized
}

bool Encoder::Add(SiameseOriginalPacket& packet) {
    if (packet.Data == nullptr || packet.DataBytes == 0) {
        return false;
    }

    // Store a copy of the packet in the window
    if (packet.PacketNum >= SIAMESE_MAX_PACKETS) {
        return false;
    }

    OriginalPacket op;
    op.Data = new uint8_t[packet.DataBytes];
    std::memcpy(op.Data, packet.Data, packet.DataBytes);
    op.DataBytes = packet.DataBytes;
    op.PacketNum = packet.PacketNum;

    originalPackets[packet.PacketNum] = op;
    ++nextPacketNum;

    return true;
}

bool Encoder::Retransmit(SiameseOriginalPacket& packet) {
    // For now, treat this the same as Add
    return Add(packet);
}

bool Encoder::GetRecoveryPacket(unsigned id, unsigned char* buffer, unsigned int bufferSize, unsigned int& usedBytes) {
    if (originalPackets.empty() || buffer == nullptr || bufferSize == 0) {
        return false;
    }

    // Generate XOR of all original packets
    memset(buffer, 0, bufferSize);
    unsigned int actualUsed = 0;

    for (const auto& [num, pkt] : originalPackets) {
        if (pkt.DataBytes > bufferSize) {
            continue; // skip too-large packets
        }
        gf256_add_mem(buffer, pkt.Data, pkt.DataBytes);
        if (pkt.DataBytes > actualUsed) {
            actualUsed = pkt.DataBytes;
        }
    }

    usedBytes = actualUsed;
    return true;
}

bool Encoder::Acknowledge(const unsigned char* data, unsigned int size, unsigned int& acknowledgedCount) {
    // Simplified mock: Assume 1 ack per call
    acknowledgedCount = 1;
    return true;
}

Encoder::~Encoder() {
    for (auto& [num, pkt] : originalPackets) {
        delete[] pkt.Data;
    }
    originalPackets.clear();
}

} // namespace siamese
