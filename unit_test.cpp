#include "siamese.h"
#include "SiameseEncoder.h"
#include <iostream>

int main() {
    siamese::Encoder encoder;
    siamese::SiameseOriginalPacket packet;

    // Example usage
    if (encoder.Add(packet) == siamese::Siamese_Success) {
        std::cout << "Packet added successfully." << std::endl;
    }

    if (encoder.Retransmit(packet) == siamese::Siamese_Success) {
        std::cout << "Packet retransmitted successfully." << std::endl;
    }

    unsigned char buffer[256];
    unsigned int bytesWritten = 0;
    if (encoder.GetRecoveryPacket(1, buffer, sizeof(buffer), bytesWritten) == siamese::Siamese_Success) {
        std::cout << "Recovery packet generated, bytes written: " << bytesWritten << std::endl;
    }

    return 0;
}
