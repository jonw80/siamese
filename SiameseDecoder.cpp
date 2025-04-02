/** \file
    \brief Siamese FEC Implementation: Decoder
    \copyright Copyright (c) 2017 Christopher A. Taylor.  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Siamese nor the names of its contributors be
      used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#include "SiameseDecoder.h"
#include "SiameseSerializers.h"
#include "GrowingAlignedDataBuffer.h"
#include "siamese.h"  // for SiameseResult
#include "SiameseConstants.h"  // for the packet limits
#include "DecoderPacketWindow.h" // For DecoderPacketWindow
#include "SiameseResult.h" // For SiameseResult

namespace siamese {

#ifdef SIAMESE_DECODER_DUMP_VERBOSE
    static logger::Channel Logger("Decoder", logger::Level::Debug);
#else
    static logger::Channel Logger("Decoder", logger::Level::Silent);
#endif


//------------------------------------------------------------------------------
// Decoder

Decoder::Decoder() noexcept {
    // Initialize members as needed
}

void Decoder::SomeFunction() noexcept {
    // Implement the function
}

SiameseResult Decoder::Get(SiameseOriginalPacket& packetOut) {
    if (Window.EmergencyDisabled) {
        return SiameseResult::Failure;
    }

    const unsigned element = Window.ColumnToElement(packetOut.packetNum);
    OriginalPacket* original = Window.GetWindowElement(element);

    if (!original) {
        packetOut.data = nullptr;
        packetOut.dataBytes = 0;
        return SiameseResult::Failure;
    }

    packetOut.data = original->Buffer.data;
    packetOut.dataBytes = original->Buffer.length;
    return SiameseResult::Success;
}

} // namespace siamese
