#ifndef SIAMESE_DECODER_H
#define SIAMESE_DECODER_H

#include <stdint.h>
#include "SiameseTypes.h"
#include "pktalloc.h" // For pktalloc::Allocator
#include "RecoveryPacketList.h" // For RecoveryPacketList
#include "DecoderPacketWindow.h" // For DecoderPacketWindow
#include "CheckedRegionState.h" // For CheckedRegionState
#include "RecoveryMatrixState.h" // For RecoveryMatrixState

namespace siamese {

enum {
    SiameseDecoderStats_AckCount,
    SiameseDecoderStats_AckBytes,
    SiameseDecoderStats_OriginalCount,
    SiameseDecoderStats_OriginalBytes,
    SiameseDecoderStats_DupedOriginalCount,
    SiameseDecoderStats_RecoveryCount,
    SiameseDecoderStats_RecoveryBytes,
    SiameseDecoderStats_DupedRecoveryCount,
    SiameseDecoderStats_SolveFailCount,
    SiameseDecoderStats_SolveSuccessCount,
    SiameseDecoderStats_MemoryUsed,
    SiameseDecoderStats_Count
};

struct DecoderStats {
    uint64_t Counts[SiameseDecoderStats_Count];
};

class Decoder {
    public:
        SiameseResult AddRecovery(const SiameseRecoveryPacket& packet);

    private:
        RecoveryPacketList RecoveryPackets;
        pktalloc::Allocator TheAllocator;
        DecoderPacketWindow Window;
        CheckedRegionState CheckedRegion;
        RecoveryMatrixState RecoveryMatrix;
        DecoderStats Stats;
};

} // namespace siamese

#endif // SIAMESE_DECODER_H