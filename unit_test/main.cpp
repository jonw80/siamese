#include <iostream>
#include <chrono>
#include "siamese.h"

int main() {
    const int num_packets = 100000; // Large enough to show performance

    SiameseEncoder encoder = siamese_encoder_create();
    if (!encoder) {
        std::cerr << "Failed to create encoder\n";
        return 1;
    }

    // Create a dummy packet
    char buffer[256] = {};
    for (int i = 0; i < 256; ++i) buffer[i] = i;

    SiameseOriginalPacket packet;
    packet.data = buffer;
    packet.dataBytes = sizeof(buffer);

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Encode many packets
    for (int i = 0; i < num_packets; ++i) {
        packet.packetNum = i;
        if (siamese_encoder_add(encoder, &packet) != Siamese_Success) {
            std::cerr << "Add failed at packet " << i << "\n";
            return 1;
        }
    }

    // End timer
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_sec = std::chrono::duration<double>(end - start).count();

    std::cout << "Encoded " << num_packets << " packets in "
              << elapsed_sec << " seconds\n";

    double packets_per_sec = num_packets / elapsed_sec;
    std::cout << "Throughput: " << packets_per_sec << " packets/sec\n";

    siamese_encoder_destroy(encoder);
    return 0;
}
