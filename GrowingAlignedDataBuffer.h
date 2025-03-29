#ifndef SIAMESE_GROWINGALIGNEDDATABUFFER_H
#define SIAMESE_GROWINGALIGNEDDATABUFFER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>

class GrowingAlignedDataBuffer
{
public:
    GrowingAlignedDataBuffer();
    ~GrowingAlignedDataBuffer();

    bool Resize(unsigned bytes);
    bool Append(const uint8_t* data, unsigned bytes);
    bool GrowZeroPadded(unsigned bytes);

    void* GetData() const { return Data; }
    unsigned GetSize() const { return Size; }

    void Clear();

private:
    uint8_t* Data;
    unsigned Size;
    unsigned Capacity;
};

#endif // SIAMESE_GROWINGALIGNEDDATABUFFER_H
