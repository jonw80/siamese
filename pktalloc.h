#ifndef PKTALLOC_H
#define PKTALLOC_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace pktalloc {

constexpr size_t PKTALLOC_ALIGN_BYTES = 16; // Define alignment constant

enum class ReallocType {
    Uninitialized,
    ZeroInitialized
};

class Allocator {
public:
    uint8_t* Allocate(size_t size) { // Changed return type to uint8_t*
        return static_cast<uint8_t*>(std::malloc(size));
    }

    void Free(void* ptr) {
        std::free(ptr);
    }

    uint8_t* Reallocate(void* ptr, size_t newSize, ReallocType reallocType) { // Changed return type to uint8_t*
        if (reallocType == ReallocType::ZeroInitialized) {
            void* newPtr = std::calloc(1, newSize);
            if (ptr) {
                std::memcpy(newPtr, ptr, newSize);
                std::free(ptr);
            }
            return static_cast<uint8_t*>(newPtr);
        }
        return static_cast<uint8_t*>(std::realloc(ptr, newSize));
    }
};

inline size_t NextAlignedOffset(size_t size) {
    const size_t alignment = PKTALLOC_ALIGN_BYTES; // Use defined alignment
    return (size + alignment - 1) & ~(alignment - 1);
}

} // namespace pktalloc

#endif // PKTALLOC_H
