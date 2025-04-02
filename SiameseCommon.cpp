// SiameseCommon.cpp
// Siamese FEC Implementation: Codec Common Definitions

#include "SiameseCommon.h"
#include "SiameseSerializers.h"
#include "SiameseTypes.h"

namespace siamese {


//------------------------------------------------------------------------------
// GrowingAlignedByteMatrix

void GrowingAlignedByteMatrix::Free(pktalloc::Allocator* allocator)
{
    SIAMESE_DEBUG_ASSERT(allocator);
    if (Data)
    {
        allocator->Free(Data);
        Data = nullptr;
        AllocatedRows = 0;
        AllocatedColumns = 0;
    }
}

bool GrowingAlignedByteMatrix::Initialize(pktalloc::Allocator* allocator, unsigned rows, unsigned columns)
{
    Rows = rows;
    Columns = columns;
    AllocatedRows = rows + kExtraRows;
    AllocatedColumns = pktalloc::NextAlignedOffset(columns + kMinExtraColumns);

    Data = allocator->Reallocate(Data, AllocatedRows * AllocatedColumns, pktalloc::Realloc::Uninitialized);
    return Data != nullptr;
}

bool GrowingAlignedByteMatrix::Resize(pktalloc::Allocator* allocator, unsigned rows, unsigned columns)
{
    SIAMESE_DEBUG_ASSERT(allocator && rows > 0 && columns > 0 && columns <= kColumnPeriod);

    if (rows <= AllocatedRows && columns <= AllocatedColumns)
    {
        Rows = rows;
        Columns = columns;
        return true;
    }

    const unsigned newAllocatedRows = rows + kExtraRows;
    const unsigned newAllocatedColumns = pktalloc::NextAlignedOffset(columns + kMinExtraColumns);

    uint8_t* newBuffer = allocator->Allocate(newAllocatedRows * newAllocatedColumns);
    if (!newBuffer)
    {
        Free(allocator);
        return false;
    }

    if (Data)
    {
        const unsigned oldColumns = Columns;
        const unsigned oldRows = Rows;
        const unsigned oldStride = AllocatedColumns;

        unsigned copyCount = oldColumns;
        if (copyCount > columns)
        {
            SIAMESE_DEBUG_BREAK(); // Should never happen
            copyCount = columns;
        }

        uint8_t* destRow = newBuffer;
        uint8_t* srcRow = Data;

        for (unsigned i = 0; i < oldRows; ++i, destRow += newAllocatedColumns, srcRow += oldStride)
            memcpy(destRow, srcRow, copyCount);

        allocator->Free(Data);
    }

    AllocatedRows = newAllocatedRows;
    AllocatedColumns = newAllocatedColumns;
    Rows = rows;
    Columns = columns;
    Data = newBuffer;
    return true;
}


//------------------------------------------------------------------------------
// OriginalPacket

unsigned OriginalPacket::Initialize(pktalloc::Allocator* allocator, const SiameseOriginalPacket& packet)
{
    SIAMESE_DEBUG_ASSERT(allocator && packet.data && packet.dataBytes > 0 && packet.packetNum < kColumnPeriod);

    const unsigned bufferSize = kMaxPacketLengthFieldBytes + packet.dataBytes;
    if (!Buffer.Initialize(allocator, bufferSize))
        return 0;

    HeaderBytes = SerializeHeader_PacketLength(packet.dataBytes, Buffer.Data);
    SIAMESE_DEBUG_ASSERT(HeaderBytes <= kMaxPacketLengthFieldBytes);

    memcpy(Buffer.Data + HeaderBytes, packet.data, packet.dataBytes);
    Buffer.Bytes = HeaderBytes + packet.dataBytes;

    Column = packet.packetNum;

    return HeaderBytes;
}

} // namespace siamese
