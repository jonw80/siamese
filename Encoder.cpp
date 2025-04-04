#include "Encoder.h"
#include "GF256Tables.h"
#include "gf256.h"
#include <vector>
#include <cstring>

namespace siamese {

Encoder::Encoder() {
    gf256_init(); // Ensure GF256 tables are initialized
    packet_count = 0;
}

bool Encoder::Add(SiameseOriginalPacket& packet) {
    if (packet.Data == nullptr || packet.DataBytes == 0)
        return false;

    OriginalPacket op;
    op.Data.assign(packet.Data, packet.Data + packet.DataBytes);
    op.PacketNum = packet.PacketNum;
    op.DataBytes = packet.DataBytes;

    originals.push_back(std::move(op));
    ++packet_count;

    return true;
}

bool Encoder::Retransmit(SiameseOriginalPacket& packet) {
    // For now, treat retransmit like a re-add
    return Add(packet);
}

bool Encoder::GetRecoveryPacket(unsigned int id, unsigned char* buffer, unsigned int bufferSize, unsigned int& packetSize) {
    if (originals.empty() || bufferSize == 0)
        return false;

    unsigned int size = originals[0].DataBytes;
    if (bufferSize < size)
        return false;

    memset(buffer, 0, size);

    for (size_t i = 0; i < originals.size(); ++i) {
        uint8_t coefficient = gf256_exp((id + i) % 255); // Simple coefficient variation
        gf256_muladd_mem(buffer, coefficient, originals[i].Data.data(), size);
    }

    packetSize = size;
    return true;
}

bool Encoder::Acknowledge(const unsigned char* data, unsigned int size, unsigned int& acknowledgedCount) {
    // Simulate ACK logic (optional depending on protocol)
    acknowledgedCount = packet_count;
    return true;
}

} // namespace siamese
