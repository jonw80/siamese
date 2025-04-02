#ifndef CHECKED_REGION_STATE_H
#define CHECKED_REGION_STATE_H

namespace siamese {

class RecoveryMatrixState;

class CheckedRegionState {
public:
    unsigned ElementStart = 0;
    unsigned NextCheckStart = 0;
    RecoveryMatrixState* RecoveryMatrix = nullptr;

    void Reset() {
        ElementStart = 0;
        NextCheckStart = 0;
    }
};

} // namespace siamese

#endif // CHECKED_REGION_STATE_H
