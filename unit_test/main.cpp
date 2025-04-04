#include <iostream>
#include <chrono>
#include "siamese.h"
#include <cstring>

int main() {
    SiameseEncoder encoder = siamese_encoder_create();
    if (!encoder) {
        std::cerr << "Failed to create encoder." << std::endl;
        return 1;
    }

    const int num_packets = 100000;
    const int packet_size = 256;

    uint8_t data_pool[num_packets][packet_size];

    // Fill and add packets
    for (int i = 0; i < num_packets; ++i) {
        memset(data_pool[i], i % 256, packet_size);

        SiameseOriginalPacket packet;
        packet.Data = data_pool[i];
        packet.DataBytes = packet_size;
        packet.PacketNum = i;

        SiameseResult result = siamese_encoder_add(encoder, &packet);
        if (result != Siamese_Success) {
            std::cerr << "Add failed at packet " << i << std::endl;
            return 2;
        }
    }

    SiameseRecoveryPacket recovery;
    uint8_t recovery_buffer[packet_size];
    recovery.Data = recovery_buffer;
    recovery.DataBytes = packet_size;

    auto start = std::chrono::high_resolution_clock::now();

    SiameseResult encode_result = siamese_encode(encoder, &recovery);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    if (encode_result != Siamese_Success) {
        std::cerr << "GetRecoveryPacket failed." << std::endl;
        return 3;
    }

    std::cout << "Encoded " << num_packets << " packets and generated 1 recovery packet in "
              << elapsed.count() << " seconds" << std::endl;
    std::cout << "Throughput: "
              << (num_packets / elapsed.count()) << " packets/sec" << std::endl;

    siamese_encoder_free(encoder);
    return 0;
}
