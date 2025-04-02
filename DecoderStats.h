#ifndef DECODER_STATS_H
#define DECODER_STATS_H

namespace siamese {

struct DecoderStats { // Changed from class to struct to match usage
    void Reset() {}
    void Log() const {}
};

} // namespace siamese

#endif // DECODER_STATS_H
