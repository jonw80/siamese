#ifndef RECOVERY_PACKET_LIST_H
#define RECOVERY_PACKET_LIST_H

#include <cstdint>
#include <vector>

namespace siamese {

struct RecoveryPacket {
    uint8_t* Data = nullptr;
    unsigned DataBytes = 0;
    unsigned ElementStart = 0;
    unsigned ElementEnd = 0;

    void Free() {
        if (Data) {
            std::free(Data);
            Data = nullptr;
            DataBytes = 0;
        }
    }
};

class RecoveryPacketList {
public:
    std::vector<RecoveryPacket*> Packets;

    ~RecoveryPacketList() {
        Clear();
    }

    void Insert(RecoveryPacket* packet) {
        Packets.push_back(packet);
    }

    void DeletePacketsBefore(unsigned element) {
        auto it = Packets.begin();
        while (it != Packets.end() && (*it)->ElementEnd <= element) {
            (*it)->Free();
            delete *it;
            it = Packets.erase(it);
        }
    }

    void Clear() {
        for (auto* packet : Packets) {
            packet->Free();
            delete packet;
        }
        Packets.clear();
    }
};

} // namespace siamese

#endif // RECOVERY_PACKET_LIST_H
