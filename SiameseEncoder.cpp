#include "SiameseEncoder.h"

namespace siamese {

Encoder::Encoder() {}

SiameseResult Encoder::Add(SiameseOriginalPacket& packet) { return 0; }
SiameseResult Encoder::Retransmit(SiameseOriginalPacket& packet) { return 0; }
SiameseResult Encoder::Acknowledge(const uint8_t*, unsigned, unsigned&) { return 0; }
SiameseResult Encoder::GetRecoveryPacket(unsigned, uint8_t*, unsigned, unsigned&) { return 0; }

}
