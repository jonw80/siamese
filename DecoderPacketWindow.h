#ifndef DECODER_PACKET_WINDOW_H
#define DECODER_PACKET_WINDOW_H

#include <vector>
#include "SiameseTypes.h"

namespace siamese {

class DecoderPacketWindow {
public:
    pktalloc::Allocator* TheAllocator = nullptr;
    DecoderStats* Stats = nullptr;
    CheckedRegionState* CheckedRegion = nullptr;
    RecoveryPacketList* RecoveryPackets = nullptr;
    RecoveryMatrixState* RecoveryMatrix = nullptr;

    unsigned Count = 0;

    bool GrowWindow(unsigned windowElementEnd) {
        Count = windowElementEnd;
        return true;
    }

    unsigned ColumnToElement(unsigned column) const {
        return column % Count;
    }

    bool InvalidElement(unsigned element) const {
        return element >= Count;
    }
};

} // namespace siamese

#endif // DECODER_PACKET_WINDOW_H
