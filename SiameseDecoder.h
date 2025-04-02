#ifndef SIAMESE_DECODER_H
#define SIAMESE_DECODER_H

#include <stdint.h>

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
        // ... other methods
    };
    
} // namespace siamese

#endif // SIAMESE_DECODER_H