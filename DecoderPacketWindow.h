#ifndef DECODER_PACKET_WINDOW_H
#define DECODER_PACKET_WINDOW_H

#include <vector>
#include "SiameseTypes.h"
#include "SiameseDecoder.h" // Ensure DecoderStats and CheckedRegionState are included
#include "DecoderStats.h" // Include the header for DecoderStats
#include "RecoveryMatrixState.h" // Ensure RecoveryMatrixState is included
#include "SiameseResult.h" // For SiameseResult

namespace siamese {

struct OriginalPacket {
    struct {
        const uint8_t* data;
        unsigned length;
    } Buffer;
};

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
