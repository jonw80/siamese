#ifndef DECODER_PACKET_WINDOW_H
#define DECODER_PACKET_WINDOW_H

#include <vector>
#include "pktalloc.h" // Include pktalloc for Allocator
#include "RecoveryPacketList.h" // Include RecoveryPacketList
#include "RecoveryMatrixState.h" // Include RecoveryMatrixState
#include "SiameseResult.h" // For SiameseResult

namespace siamese {

class DecoderPacketWindow {
public:
    pktalloc::Allocator* TheAllocator = nullptr;
    DecoderStats* Stats = nullptr;
    CheckedRegionState* CheckedRegion = nullptr;
    RecoveryPacketList* RecoveryPackets = nullptr;
    RecoveryMatrixState* RecoveryMatrix = nullptr;

    unsigned Count = 0;
    bool EmergencyDisabled = false;

    bool GrowWindow(unsigned windowElementEnd) {
        Count = windowElementEnd;
        return true;
    }

    unsigned ColumnToElement(unsigned column) const {
        return column;
    }

    bool InvalidElement(unsigned element) const {
        return element >= Count;
    }

    OriginalPacket* GetWindowElement(unsigned element) {
        return nullptr; // Example return value
    }
};

} // namespace siamese

#endif // DECODER_PACKET_WINDOW_H
