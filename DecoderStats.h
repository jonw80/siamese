#ifndef DECODER_STATS_H
#define DECODER_STATS_H

namespace siamese {

class DecoderStats {
public:
    unsigned Counts[10] = {0}; // Initialize the array

    DecoderStats() noexcept {
        for (unsigned i = 0; i < 10; ++i) {
            Counts[i] = 0;
        }
    }
};

} // namespace siamese

#endif // DECODER_STATS_H
