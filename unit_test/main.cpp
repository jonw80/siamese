#include <iostream>
#include <chrono>
#include <cstring>
#include "siamese.h"

int main() {
    SiameseEncoder encoder = siamese_encoder_create();
    if (!encoder) {
        std::cerr << "Failed to create encoder." << std::endl;
        return 1;
    }

    const int num_packets = 100000;
    const int packet_size = 256;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_packets; ++i) {
        uint8_t buffer[256];
        memset(buffer, i % 256, packet_size);

        SiameseOriginalPacket packet;
        packet.data = buffer;
        packet.dataBytes = packet_size;
        packet.packetNum = i;

        siamese_encoder_add(encoder, &packet);
    }

    uint8_t recovery[256];
    unsigned used = 0;
    siamese::SiameseResult result = siamese_encoder_get_recovery(encoder, 0, recovery, 256, &used);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Encoded " << num_packets << " packets in "
              << elapsed.count() << " seconds" << std::endl;
    std::cout << "Throughput: "
              << (num_packets / elapsed.count()) << " packets/sec" << std::endl;

    siamese_encoder_free(encoder);
    return 0;
}
