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
    uint8_t recovery_buffer[packet_size];

    SiameseRecoveryPacket recovery_packet;
    recovery_packet.Data = recovery_buffer;
    recovery_packet.DataBytes = packet_size;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_packets; ++i) {
        uint8_t buffer[packet_size];
        memset(buffer, i % 256, packet_size);

        SiameseOriginalPacket packet;
        packet.Data = buffer;
        packet.DataBytes = packet_size;
        packet.PacketNum = i;

        SiameseResult result = siamese_encoder_add(encoder, &packet);
        if (result != Siamese_Success && result != Siamese_NeedMoreData) {
            std::cerr << "Add failed at packet " << i << " with result: " << result << std::endl;
            return 1;
        }

        // Actually encode to generate a recovery packet
        SiameseResult enc_result = siamese_encode(encoder, &recovery_packet);
        if (enc_result != Siamese_Success && enc_result != Siamese_NeedMoreData) {
            std::cerr << "Encode failed at packet " << i << " with result: " << enc_result << std::endl;
            return 1;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Encoded " << num_packets << " packets and generated recovery packets in "
              << elapsed.count() << " seconds" << std::endl;
    std::cout << "Throughput: "
              << (num_packets / elapsed.count()) << " packets/sec" << std::endl;

    siamese_encoder_free(encoder);
    return 0;
}
