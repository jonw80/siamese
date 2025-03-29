#include "siamese.h"
#include "SiameseEncoder.h"

using namespace siamese;

extern "C" {

SiameseResult siamese_encoder_add(SiameseEncoder encoder, SiameseOriginalPacket* packet) {
    return encoder->Add(*packet);
}

}
