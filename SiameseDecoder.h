#ifndef SIAMESE_DECODER_H
#define SIAMESE_DECODER_H

#include <cstdint>
#include "siamese.h" // For SiameseOriginalPacket and SiameseRecoveryPacket
#include "SiameseResult.h" // For SiameseResult
#include "DecoderPacketWindow.h" // For DecoderPacketWindow
#include "DecoderStats.h" // For DecoderStats

namespace siamese {

class Decoder {
public:
    Decoder() noexcept;
    void SomeFunction() noexcept;
    SiameseResult Get(SiameseOriginalPacket& packetOut);
    SiameseResult AddRecovery(const SiameseRecoveryPacket& packet);

private:
    DecoderPacketWindow Window;
    DecoderStats Stats;
};

} // namespace siamese

#endif // SIAMESE_DECODER_H