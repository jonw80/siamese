#ifndef DECODER_PACKET_WINDOW_H
#define DECODER_PACKET_WINDOW_H

#include <vector>
#include "pktalloc.h" // Include pktalloc for Allocator
#include "RecoveryPacketList.h" // Include RecoveryPacketList
#include "RecoveryMatrixState.h" // Include RecoveryMatrixState
#include "SiameseResult.h" // For SiameseResult
#include "DecoderStats.h" // Include DecoderStats
#include "SiameseCommon.h" // Include OriginalPacket

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
        (void)windowElementEnd; // Suppress unused parameter warning
        Count = windowElementEnd;
        return true;
    }

    unsigned ColumnToElement(unsigned column) const {
        (void)column; // Suppress unused parameter warning
        return column;
    }

    bool InvalidElement(unsigned element) const {
        (void)element; // Suppress unused parameter warning
        return element >= Count;
    }

    OriginalPacket* GetWindowElement(unsigned element) {
        (void)element; // Suppress unused parameter warning
        return nullptr; // Example return value
    }
};

} // namespace siamese

#endif // DECODER_PACKET_WINDOW_H
