#include "siamese.h"
#include "SiameseEncoder.h"

int main() {
    SiameseEncoder encoder = siamese_encoder_create();

    SiameseOriginalPacket packet;
    packet.data = nullptr;
    packet.dataBytes = 0;
    packet.packetNum = 0;

    if (siamese_encoder_add(encoder, &packet) == Siamese_Success) {
        // Test passed for Add
    }

    if (siamese_encoder_retransmit(encoder, &packet) == Siamese_Success) {
        // Test passed for Retransmit
    }

    uint8_t buffer[256];
    unsigned bytesWritten = 0;
    if (siamese_encoder_get_recovery(encoder, 1, buffer, sizeof(buffer), &bytesWritten) == Siamese_Success) {
        // Test passed for GetRecoveryPacket
    }

    siamese_encoder_free(encoder);
    return 0;
}
