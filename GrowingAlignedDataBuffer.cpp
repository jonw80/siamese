#include "GrowingAlignedDataBuffer.h"

GrowingAlignedDataBuffer::GrowingAlignedDataBuffer()
    : Data(nullptr), Size(0), Capacity(0) {}

GrowingAlignedDataBuffer::~GrowingAlignedDataBuffer()
{
    Clear();
}

bool GrowingAlignedDataBuffer::Resize(unsigned bytes)
{
    if (bytes <= Capacity)
    {
        Size = bytes;
        return true;
    }

    uint8_t* newData = (uint8_t*)aligned_alloc(16, bytes);
    if (!newData) return false;

    if (Data)
    {
        memcpy(newData, Data, Size);
        free(Data);
    }

    Data = newData;
    Size = bytes;
    Capacity = bytes;
    return true;
}

bool GrowingAlignedDataBuffer::Append(const uint8_t* data, unsigned bytes)
{
    if (!Resize(Size + bytes))
        return false;

    memcpy((uint8_t*)Data + (Size - bytes), data, bytes);
    return true;
}

bool GrowingAlignedDataBuffer::GrowZeroPadded(unsigned bytes)
{
    if (bytes <= Size)
        return true;

    unsigned oldSize = Size;
    if (!Resize(bytes))
        return false;

    memset((uint8_t*)Data + oldSize, 0, bytes - oldSize);
    return true;
}

void GrowingAlignedDataBuffer::Clear()
{
    if (Data)
    {
        free(Data);
        Data = nullptr;
    }
    Size = 0;
    Capacity = 0;
}
