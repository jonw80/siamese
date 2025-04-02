#include "siamese.h"
#include <iostream>

int main() {
    siamese::Encoder encoder;
    SiameseOriginalPacket packet;

    // Example usage
    encoder.Add(packet);
    encoder.Retransmit(packet);

    unsigned char buffer[256];
    unsigned int bytesWritten = 0;
    if (encoder.GetRecoveryPacket(1, buffer, sizeof(buffer), bytesWritten)) {
        std::cout << "Recovery packet generated, bytes written: " << bytesWritten << std::endl;
    }

    return 0;
}
