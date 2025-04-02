#ifndef RECOVERY_MATRIX_STATE_H
#define RECOVERY_MATRIX_STATE_H

#include "pktalloc.h" // Include pktalloc for Allocator
#include "CheckedRegionState.h" // Include CheckedRegionState

namespace siamese {

// Forward declaration to break circular dependency
class DecoderPacketWindow;

class RecoveryMatrixState {
public:
    pktalloc::Allocator* TheAllocator = nullptr;
    DecoderPacketWindow* Window = nullptr;
    CheckedRegionState* CheckedRegion = nullptr;

    void Reset() {
        // Placeholder for reset logic
    }
};

} // namespace siamese

#endif // RECOVERY_MATRIX_STATE_H
