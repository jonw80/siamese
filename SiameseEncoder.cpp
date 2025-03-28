/** \file
    \brief Siamese Encoder Implementation
    \copyright Copyright (c) 2017 Christopher A. Taylor.  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Siamese nor the names of its contributors may be
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

#include "SiameseEncoder.h"
#include "SiameseTools.h"

namespace siamese {


//------------------------------------------------------------------------------
// EncoderPacketWindow

EncoderPacketWindow::EncoderPacketWindow()
{
    TheAllocator = nullptr;
    Stats = nullptr;
    ClearWindow();
}

void EncoderPacketWindow::ClearWindow()
{
    CountLock.Enter();
    Count = 0;
    CountLock.Leave();

    // Reset for recovery packets
    LongestPacket = 0;
    ColumnStart = 0;
    SumColumnStart = 0;
    FirstUnremovedElement = 0;
    SumStartElement = 0;
    SumEndElement = 0;
    SumErasedCount = 0;
    EmergencyDisabled = false;
}

SiameseResult EncoderPacketWindow::Add(SiameseOriginalPacket& packet)
{
    if (packet.DataBytes < SIAMESE_MIN_PACKET_BYTES ||
        packet.DataBytes > SIAMESE_MAX_PACKET_BYTES)
    {
        return Siamese_BadInput;
    }

    // Initialize window if it is empty
    if (Count == 0)
    {
        ColumnStart = packet.PacketNum;
    }

    // Check if window is full
    const unsigned kMaxElements = SIAMESE_MAX_PACKETS;
    if (Count >= kMaxElements)
        return Siamese_MaxPacketsReached;

    // Calculate element index for this packet
    unsigned element = (packet.PacketNum - ColumnStart) & SIAMESE_PACKET_NUM_MASK;

    // If packet does not fit in window
    if (element >= kMaxElements)
        return Siamese_MaxPacketsReached;

    // If this is a retransmission or late arrival, ignore it
    if (element < Count)
    {
        OriginalPacket* original = GetWindowElement(element);
        if (original->Column == packet.PacketNum)
            return Siamese_DuplicateData;
    }
    else if (element > Count)
    {
        // Element is being added ahead of window
        while (Count < element)
        {
            // Fill in intervening elements
            OriginalPacket* original = GetWindowElement(Count);
            original->Data = nullptr;
            original->Column = (ColumnStart + Count) & SIAMESE_PACKET_NUM_MASK;
            original->LengthBytes = 0;

            if (!Stats->OnAddOriginal(this, true))
                return Siamese_MaxPacketsReached;

            ++Count;
        }
    }

    // Fill in the window element
    OriginalPacket* original = GetWindowElement(element);
    original->Column = packet.PacketNum;
    original->LengthBytes = packet.DataBytes;
    if (LongestPacket < packet.DataBytes)
        LongestPacket = packet.DataBytes;

    original->Data = TheAllocator->Allocate(original->LengthBytes);
    if (!original->Data)
        return Siamese_OutOfMemory;

    memcpy(original->Data, packet.Data, original->LengthBytes);

    if (element >= Count)
    {
        if (!Stats->OnAddOriginal(this, false))
            return Siamese_MaxPacketsReached;

        ++Count;
    }

    return Siamese_Success;
}

void EncoderPacketWindow::StartNewWindow(unsigned column)
{
    ColumnStart = column;
    SumColumnStart = column;

    CountLock.Enter();
    Count = 0;
    CountLock.Leave();

    FirstUnremovedElement = 0;
    SumStartElement = 0;
    SumEndElement = 0;
    SumErasedCount = 0;
}

void EncoderPacketWindow::RemoveBefore(unsigned firstKeptColumn)
{
    // Clip column to within the range of window columns
    unsigned elementToRemoveBefore = 0;
    if (firstKeptColumn > ColumnStart)
        elementToRemoveBefore = SubtractColumns(firstKeptColumn, ColumnStart);
    if (elementToRemoveBefore > Count)
        elementToRemoveBefore = Count;

    // Remove obsolete data
    for (unsigned i = FirstUnremovedElement; i < elementToRemoveBefore; ++i)
    {
        OriginalPacket* original = GetWindowElement(i);
        if (original->Data)
            TheAllocator->Free(original->Data);
        original->Data = nullptr;

        Stats->OnRemoveOriginal();
    }

    FirstUnremovedElement = elementToRemoveBefore;
}

void EncoderPacketWindow::ResetSums(unsigned elementStart)
{
    // Set start of summing range
    SumStartElement = elementStart;

    // Reset SumColumnStart
    SumColumnStart = (ColumnStart + SumStartElement) & SIAMESE_PACKET_NUM_MASK;

    // Clear sum block(s)
    for (unsigned i = 0; i < kColumnSumCount; ++i)
    {
        for (unsigned j = 0; j < kColumnSumCount; ++j)
        {
            GrowingAlignedDataBuffer& sums = Sums[i][j];
            sums.Resize(0);
        }
    }

    SumEndElement = SumStartElement;
    SumErasedCount = 0;
}

void EncoderPacketWindow::RemoveElements()
{
    // Remove any elements that have been removed
    if (FirstUnremovedElement > 0)
    {
        // Reset the summing range when packets are removed
        ResetSums(FirstUnremovedElement);

        // Update count
        CountLock.Enter();
        Count -= FirstUnremovedElement;
        CountLock.Leave();

        // Update column start
        ColumnStart = (ColumnStart + FirstUnremovedElement) & SIAMESE_PACKET_NUM_MASK;

        // Slide window up to account for removed packets
        for (unsigned i = 0; i < Count; ++i)
        {
            WindowElements[i] = WindowElements[i + FirstUnremovedElement];
            ElementTimestamps[i] = ElementTimestamps[i + FirstUnremovedElement];
        }

        // Reset housekeeping for the window
        FirstUnremovedElement = 0;
    }
}

const GrowingAlignedDataBuffer* EncoderPacketWindow::GetSum(
    unsigned laneIndex,
    unsigned sumIndex,
    unsigned elementEnd)
{
    // If sum range is empty:
    if (SumEndElement >= elementEnd)
        return &Sums[laneIndex][sumIndex];

    // If sum is for a lane that is unused or not added:
    GrowingAlignedDataBuffer& sums = Sums[laneIndex][sumIndex];

    // For each original packet that needs to be summed:
    for (unsigned element = SumEndElement; element < elementEnd; ++element)
    {
        OriginalPacket* original = GetWindowElement(element);

        // If data is not available:
        if (!original->Data || !original->LengthBytes)
        {
            ++SumErasedCount;
            continue;
        }

        // If original packet is too small to contain a byte for this lane:
        const unsigned laneByteIndex = laneIndex;
        if (original->LengthBytes <= laneByteIndex)
            continue;

        // If this is the first packet in the summation range:
        if (sums.GetSize() <= 0)
        {
            // Allocate space for a sum
            if (!sums.Append(original->Data, original->LengthBytes))
                return nullptr;
        }
        else
        {
            const unsigned minBytes = (original->LengthBytes < sums.GetSize()) ?
                original->LengthBytes : sums.GetSize();

            uint8_t coefficient;
            if (sumIndex == 0)
                coefficient = 1;
            else
            {
                // Sum 1 = Sum of encoded with original columns
                // Column values are chosen so none of them are 0
                coefficient = GetColumnValue(original->Column);

                // Sum 2 = Sum of encoded with original columns ^ 2
                // Sum 3 = Sum of encoded with original columns ^ 3
                // ...
                for (unsigned i = 1; i < sumIndex; ++i)
                    coefficient = gf256_mul(coefficient, GetColumnValue(original->Column));
            }

            // If lane is 0:
            if (laneIndex == 0)
            {
                if (coefficient == 1)
                {
                    // Just XOR
                    gf256_add_mem(sums.GetData(), original->Data, minBytes);
                }
                else
                {
                    // Muladd operation
                    gf256_muladd_mem(sums.GetData(), coefficient, original->Data, minBytes);
                }
            }
            else
            {
                const unsigned sums_size = sums.GetSize();
                uint8_t* dest_ptr = sums.GetData();
                const uint8_t* src_ptr = original->Data;

                if (coefficient == 1)
                {
                    // Just XOR
                    for (unsigned i = laneByteIndex; i < minBytes; i += GF256_STRIDE)
                        dest_ptr[i] ^= src_ptr[i];
                }
                else
                {
                    // Muladd operation
                    for (unsigned i = laneByteIndex; i < minBytes; i += GF256_STRIDE)
                        dest_ptr[i] ^= gf256_mul(coefficient, src_ptr[i]);
                }
            }
        }
    }

    return &sums;
}


//------------------------------------------------------------------------------
// EncoderAcknowledgementState

bool EncoderAcknowledgementState::OnAcknowledgementData(const uint8_t* data, unsigned bytes)
{
    const unsigned minBytes = SIAMESE_ACK_MIN_BYTES;

    // Check input args
    if (!data || bytes < minBytes)
        return false;

    /*
        Buffer format:

        The message contains a list of ranges of columns that were received,
        followed by a list of columns that were lost, sorted by sequence number.

        [0...] 32-bit Next Column Expected: First column to send next
        [4...] 8-bit Range Count (M): Number of ranges to acknowledge
        [5...] 32-bit (Range Start 0) first range column start
        [9...] 32-bit (Range End 0) first range column end - Exclusive
        ...
        [5 + 8*(M-1)...] 32-bit (Range Start M-1) last range column start
        [9 + 8*(M-1)...] 32-bit (Range End M-1) last range column end - Exclusive
        [5 + 8*M...] List of 32-bit lost columns, sorted
    */

    // Get next column expected
    uint32_t nextColumnExpected_32;
    memcpy(&nextColumnExpected_32, data, sizeof(nextColumnExpected_32));
    UInt32_ByteSwap(nextColumnExpected_32);
    NextColumnExpected = nextColumnExpected_32 & SIAMESE_PACKET_NUM_MASK;

    // Get range count
    const unsigned rangeCount = data[4];
    if (rangeCount > kMaxRangeCount)
        return false;

    // Get offset of first loss
    unsigned offset = 5 + 8 * rangeCount;
    if (offset > bytes)
        return false;

    // Copy range data
    AckRangeCount = rangeCount;
    for (unsigned i = 0; i < rangeCount; ++i)
    {
        AckRangeStart[i] = NextColumnExpected;
        AckRangeEnd[i] = NextColumnExpected;

        uint32_t rangeStart_32, rangeEnd_32;
        const unsigned startOffset = 5 + 8 * i;
        memcpy(&rangeStart_32, data + startOffset, sizeof(rangeStart_32));
        UInt32_ByteSwap(rangeStart_32);
        memcpy(&rangeEnd_32, data + startOffset + 4, sizeof(rangeEnd_32));
        UInt32_ByteSwap(rangeEnd_32);

        AckRangeStart[i] = rangeStart_32 & SIAMESE_PACKET_NUM_MASK;
        AckRangeEnd[i] = rangeEnd_32 & SIAMESE_PACKET_NUM_MASK;
    }

    // Copy negative acknowledgement data
    NackCount = 0;
    for (unsigned i = 0; offset + i * 4 + 4 <= bytes && i < kMaxNackCount; ++i)
    {
        uint32_t loss_32;
        memcpy(&loss_32, data + offset + i * 4, sizeof(loss_32));
        UInt32_ByteSwap(loss_32);

        NackColumns[NackCount++] = loss_32 & SIAMESE_PACKET_NUM_MASK;
    }

    // Restart the loss iterator
    LossIndex = 0;
    LossRangeIndex = 0;
    NeedToRetransmitOldest = false;
    FoundOldest = false;

    UpdateRTO();

    return true;
}

void EncoderAcknowledgementState::UpdateRTO()
{
    const unsigned windowStart = TheWindow->ColumnStart;

    EncoderTimestampRecord* timestamps = TheWindow->ElementTimestamps;
    const unsigned count = TheWindow->Count;

    SmoothedRTT = 0;
    RetransmitTimeoutMsec = kMinRTO;

    unsigned receivedCount = 0;
    uint64_t nowMsec = GetTimeMsec();
    const unsigned maxChecks = (count > kMaxRTOSamples) ? kMaxRTOSamples : count;

    for (unsigned i = 0; i < maxChecks; ++i)
    {
        const unsigned element = TheWindow->FirstUnremovedElement + i;

        // Stop after looking at most recent 1/4 of the window
        if (element >= count || element >= TheWindow->FirstUnremovedElement + (count / 4))
            break;

        // Get original column number
        OriginalPacket* original = TheWindow->GetWindowElement(element);
        const unsigned column = original->Column;

        // If the packet had never been sent:
        uint32_t& sendTimeMsec = *TheWindow->GetWindowElementTimestampPtr(element);
        if (sendTimeMsec == 0)
            continue;

        // Check if packet was already acknowledged
        if (IsAcknowledged(column))
        {
            // Get elapsed time since it was sent
            uint32_t deltaMsec = GetTimePastMsec(sendTimeMsec);

            // If we have a realistic value:
            if (deltaMsec > 5 && deltaMsec < 3600000) // Between 5 msec and 1 hour
            {
                receivedCount++;

                // EWMA estimation of RTT
                if (SmoothedRTT == 0)
                    SmoothedRTT = deltaMsec;
                else
                {
                    SmoothedRTT = (SmoothedRTT * 3 + deltaMsec) / 4;
                    if (SmoothedRTT < 1)
                        SmoothedRTT = 1;
                }
            }

            // Reset timer since the packet was acknowledged
            sendTimeMsec = 0;
        }
        else if (IsAcknowledgementLost(column))
        {
            // Get elapsed time since it was sent
            uint32_t deltaMsec = GetTimePastMsec(sendTimeMsec);

            // If elapsed time > 100 ms, check if we need to retransmit
            if (deltaMsec >= 100)
            {
                NeedToRetransmitOldest = true;

                // If this is the first pending packet:
                if (!FoundOldest)
                {
                    FoundOldest = true;
                    OldestColumn = column;
                }
            }
        }
    }

    if (SmoothedRTT > 0)
        RetransmitTimeoutMsec = SmoothedRTT * 3 / 2;

    if (RetransmitTimeoutMsec < kMinRTO)
        RetransmitTimeoutMsec = kMinRTO;
    else if (RetransmitTimeoutMsec > kMaxRTO)
        RetransmitTimeoutMsec = kMaxRTO;
}

bool EncoderAcknowledgementState::IsAcknowledged(unsigned column)
{
    for (unsigned i = 0; i < AckRangeCount; ++i)
    {
        if (ColumnIsInRange(AckRangeStart[i], AckRangeEnd[i], column))
            return true;
    }

    return false;
}

bool EncoderAcknowledgementState::IsAcknowledgementLost(unsigned column)
{
    // Skip columns that have not been acknowledged yet and are
    // after the next packet expected by the decoder
    if (!ColumnInRange(column, NextColumnExpected))
        return false;

    for (unsigned i = 0; i < NackCount; ++i)
    {
        if (column == NackColumns[i])
            return true;
    }

    return !IsAcknowledged(column);
}

bool EncoderAcknowledgementState::HasNegativeAcknowledgements()
{
    return (NackCount > 0);
}

bool EncoderAcknowledgementState::DecodeNextRange()
{
    // If we have been through them all:
    if (LossRangeIndex >= AckRangeCount)
        return false;

    const unsigned windowStart = TheWindow->ColumnStart;
    const unsigned windowEnd = (windowStart + TheWindow->Count) & SIAMESE_PACKET_NUM_MASK;

    // Find the range that contains windowEnd
    unsigned foundIndex = AckRangeCount;
    for (unsigned i = 0; i < AckRangeCount; ++i)
    {
        if (ColumnIsInRange(AckRangeStart[i], AckRangeEnd[i], windowEnd))
        {
            foundIndex = i;
            break;
        }
    }

    // If no range start contains windowEnd:
    if (foundIndex >= AckRangeCount)
        return false;

    // Smallest range end defines the current segment to walk through
    LossRangeIndex = foundIndex;
    return true;
}

bool EncoderAcknowledgementState::GetNextLossColumn(unsigned& columnOut)
{
    if (LossIndex < NackCount)
    {
        columnOut = NackColumns[LossIndex++];
        return true;
    }
    
    return false;
}

void EncoderAcknowledgementState::RestartLossIterator()
{
    LossIndex = 0;
    LossRangeIndex = 0;
}

void EncoderAcknowledgementState::Clear()
{
    AckRangeCount = 0;
    NackCount = 0;
    LossIndex = 0;
    LossRangeIndex = 0;
    FoundOldest = false;
    NextColumnExpected = 0;
    SmoothedRTT = 0;
    RetransmitTimeoutMsec = kMinRTO;
}


//------------------------------------------------------------------------------
// Encoder

Encoder::Encoder()
{
    Window.TheAllocator = &TheAllocator;
    Window.Stats        = &Stats;
    Ack.TheAllocator    = &TheAllocator;
    Ack.TheWindow       = &Window;
}

SiameseResult Encoder::Acknowledge(
    const uint8_t* data,
    unsigned bytes,
    unsigned& nextExpectedPacketNumOut)
{
    if (Window.EmergencyDisabled) {
        return Siamese_Disabled;
    }

    if (!Ack.OnAcknowledgementData(data, bytes)) {
        return Siamese_BadInput;
    }

    nextExpectedPacketNumOut = Ack.NextColumnExpected;
    return Siamese_Success;
}

SiameseResult Encoder::AttemptRetransmit(
    OriginalPacket* originalRetransmit,
    SiameseOriginalPacket& originalOut)
{
    if (!originalRetransmit) {
        return Siamese_InvalidInput;
    }

    if (originalRetransmit->LengthBytes > SIAMESE_MAX_PACKET_BYTES) {
        // Failed sanity check
        Logger.Error("AttemptRetransmit failed: Original length=", originalRetransmit->LengthBytes, " cannot exceed limit=", SIAMESE_MAX_PACKET_BYTES);
        Window.EmergencyDisabled = true;
        return Siamese_Disabled;
    }

    originalOut.PacketNum = originalRetransmit->Column;
    originalOut.DataBytes = originalRetransmit->LengthBytes;
    originalOut.Data = originalRetransmit->Data;

    return Siamese_Success;
}

SiameseResult Encoder::Retransmit(SiameseOriginalPacket& originalOut)
{
    if (Window.EmergencyDisabled) {
        return Siamese_Disabled;
    }

    // If there is nothing unacknowledged:
    if (Window.GetUnacknowledgedCount() <= 0)
    {
        Ack.FoundOldest = false; // Reset Ack state
        return Siamese_NothingToRetransmit;
    }

    // Calculate first window element
    const unsigned firstElement = SubtractColumns(Ack.NextColumnExpected, Window.ColumnStart);
    const unsigned count = Window.Count;

    // Respect decoder min retransmit interval
    // Which is based on how much is pending ack from the other side
    if (firstElement >= count ||
        firstElement < Window.FirstUnremovedElement ||
        count <= Window.FirstUnremovedElement)
    {
        Ack.FoundOldest = false;
        return Siamese_NothingToRetransmit;
    }

    const uint32_t retransmitMsec = Ack.RetransmitTimeoutMsec;

    // If we have a good idea of which packet to retransmit:
    if (Ack.FoundOldest)
    {
        const unsigned element = SubtractColumns(Ack.OldestColumn, Window.ColumnStart);

        if (element >= count ||
            element < Window.FirstUnremovedElement)
        {
            Ack.FoundOldest = false;
        }
        else
        {
            OriginalPacket* original = Window.GetWindowElement(element);

            // If there are no packets to retransmit or if the packet is already acknowledged
            if (!original->Data ||
                Ack.IsAcknowledged(original->Column))
            {
                Ack.FoundOldest = false;
            }
            else
            {
                uint32_t* lastSendMsecPtr = Window.GetWindowElementTimestampPtr(element);
                uint32_t elapsedMsec = GetTimePastMsec(*lastSendMsecPtr);

                // If we have not already retransmitted this recently:
                if (elapsedMsec >= retransmitMsec)
                {
                    *lastSendMsecPtr = GetTimeMsec();

                    // Retransmit oldest unacknowledged packet
                    return AttemptRetransmit(original, originalOut);
                }
            }
        }

        Ack.FoundOldest = false;
    }

    // (2) If we know some packet data was lost, use the NACK list
    if (Ack.HasNegativeAcknowledgements())
    {
        unsigned column;
        
        Ack.RestartLossIterator();
        
        while (Ack.GetNextLossColumn(column))
        {
            unsigned nackElement = Window.ColumnToElement(column);
            
            // If the packet column is not in the window:
            if (nackElement >= count ||
                nackElement < Window.FirstUnremovedElement ||
                !ColumnInRange(column, Window.ColumnStart))
            {
                continue;
            }
            
            // Get packet data
            OriginalPacket* original = Window.GetWindowElement(nackElement);
            uint32_t* lastSendMsecPtr = Window.GetWindowElementTimestampPtr(nackElement);
            uint32_t elapsedMsec = GetTimePastMsec(*lastSendMsecPtr);
            
            // If packet data is now missing:
            if (!original->Data)
                continue;
            
            // If we have not already retransmitted this recently:
            if (elapsedMsec >= retransmitMsec)
            {
                // Record time of retransmit
                *lastSendMsecPtr = GetTimeMsec();
                
                // Retransmit selected unacknowledged packet
                return AttemptRetransmit(original, originalOut);
            }
        }
    }

    // (3) Just scan for something to retransmit
    for (unsigned element = firstElement,
         elementEnd = (firstElement + kLookBehind > count) ? count : (firstElement + kLookBehind);
         element < elementEnd; ++element)
    {
        OriginalPacket* original = Window.GetWindowElement(element);
        uint32_t* lastSendMsecPtr = Window.GetWindowElementTimestampPtr(element);
        uint32_t elapsedMsec = GetTimePastMsec(*lastSendMsecPtr);

        if (!original->Data ||
            Ack.IsAcknowledged(original->Column))
        {
            continue;
        }

        // If we have not already retransmitted this recently:
        if (elapsedMsec >= retransmitMsec)
        {
            *lastSendMsecPtr = GetTimeMsec();

            // Record for faster selection next time
            Ack.FoundOldest  = true;
            Ack.OldestColumn = original->Column;

            // Retransmit packet
            return AttemptRetransmit(original, originalOut);
        }
    }

    return Siamese_NothingToRetransmit;
}

void Encoder::AddDenseColumns(unsigned laneCount, uint8_t* outDataPtr)
{
    const unsigned recoveryBytes = Window.LongestPacket;

    // For each lane:
    for (unsigned laneIndex = 0; laneIndex < laneCount; ++laneIndex)
    {
        // For each column type:
        for (unsigned sumIndex = 0; sumIndex < kColumnSumCount; ++sumIndex)
        {
            const GrowingAlignedDataBuffer* sum = Window.GetSum(laneIndex, sumIndex, Window.Count);

            const bool enabled = (sumIndex + laneIndex) % 3 != 1; // Skip some sums
            if (enabled && sum && sum->GetSize() > 0)
            {
                // Add small row
                const unsigned sumDataBytes = sum->GetSize();
                const uint8_t* sumDataPtr = sum->GetData();

                const unsigned bytes = (recoveryBytes < sumDataBytes) ? recoveryBytes : sumDataBytes;

                if (laneIndex == 0)
                {
                    memcpy(outDataPtr, sumDataPtr, bytes);
                }
                else
                {
                    unsigned reportedBytes = bytes - laneIndex;
                    if (reportedBytes > 0)
                    {
                        memcpy(outDataPtr, sumDataPtr + laneIndex, reportedBytes);
                    }
                }
            }

            outDataPtr += recoveryBytes;
        }
    }

    Window.SumEndElement = Window.Count;
}

void Encoder::AddLightColumns(unsigned laneCount, uint8_t* outDataPtr)
{
    const unsigned startElement = Window.FirstUnremovedElement;
    const unsigned count = Window.SumEndElement - startElement;

    PCGRandom prng;
    prng.Seed(0, 0xc8210b4e);

    uint8_t* buffer = (uint8_t*)TheAllocator.Allocate(Window.LongestPacket);
    if (!buffer)
    {
        Window.EmergencyDisabled = true;
        return;
    }

    const unsigned lanesToUse = (laneCount > GF256_STRIDE) ? GF256_STRIDE : laneCount;

    // For each randomly chosen element pair:
    for (unsigned rxCount = 0; rxCount < kLightCount; ++rxCount)
    {
        // Select two random elements in range
        unsigned element1 = startElement + (prng.Next() % count);
        unsigned elementRX = startElement + (prng.Next() % count);

        // Get original for element1
        const OriginalPacket* original1  = Window.GetWindowElement(element1);
        // Get original for elementRX
        const OriginalPacket* originalRX = Window.GetWindowElement(elementRX);

        // If neither has data:
        if ((!original1->Data || original1->LengthBytes <= 0) &&
            (!originalRX->Data || originalRX->LengthBytes <= 0))
        {
            continue;
        }

            // Generate a new packet from the two chosen ones with some XORs
    if (original1->Data && original1->LengthBytes > 0)
    {
        memcpy(buffer, original1->Data, original1->LengthBytes);
        if (original1->LengthBytes < Window.LongestPacket)
        {
            memset(buffer + original1->LengthBytes, 0, Window.LongestPacket - original1->LengthBytes);
        }
    }
    else
    {
        memset(buffer, 0, Window.LongestPacket);
    }

    if (originalRX->Data && originalRX->LengthBytes > 0)
    {
        const unsigned bytes = (originalRX->LengthBytes < Window.LongestPacket)
            ? originalRX->LengthBytes : Window.LongestPacket;

        gf256_add_mem(buffer, originalRX->Data, bytes);
    }

    // Copy combined data to output
    memcpy(outDataPtr, buffer, Window.LongestPacket);
    outDataPtr += Window.LongestPacket;
    }

    TheAllocator.Free(buffer);
}

SiameseResult Encoder::GetRecoveryPacket(
    unsigned packetNum,
    uint8_t* buffer,
    unsigned bufferBytes,
    unsigned& outRecoveryBytes)
{
    if (Window.EmergencyDisabled)
        return Siamese_Disabled;

    const unsigned laneCount = GF256_STRIDE;
    const unsigned columnSumCount = kColumnSumCount;
    const unsigned bytesPerRow = Window.LongestPacket;

    const unsigned requiredBytes =
        (laneCount * columnSumCount + kLightCount) * bytesPerRow;

    if (bufferBytes < requiredBytes)
        return Siamese_NeedMoreData;

    AddDenseColumns(laneCount, buffer);
    AddLightColumns(laneCount, buffer + laneCount * columnSumCount * bytesPerRow);

    outRecoveryBytes = requiredBytes;
    return Siamese_Success;
}

} // namespace siamese
