#include "siamese.h"
#include <iostream>

int main() {
    // Initialize the Siamese library
    if (siamese_init() != 0) {
        std::cerr << "Failed to initialize Siamese library." << std::endl;
        return 1;
    }

    // Create an encoder
    SiameseEncoder encoder = siamese_encoder_create();
    if (!encoder) {
        std::cerr << "Failed to create Siamese encoder." << std::endl;
        return 1;
    }

    // Free the encoder
    siamese_encoder_free(encoder);

    std::cout << "Siamese library test completed successfully." << std::endl;
    return 0;
}

#include <iostream>

int main() {
    std::cout << "Unit tests are not yet implemented." << std::endl;
    return 0;
}
