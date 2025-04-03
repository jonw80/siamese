#ifndef PKTALLOC_H
#define PKTALLOC_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace pktalloc {

#if defined(__AVX__)
    constexpr size_t PKTALLOC_ALIGN_BYTES = 32;
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    constexpr size_t PKTALLOC_ALIGN_BYTES = 16;
#else
    constexpr size_t PKTALLOC_ALIGN_BYTES = 8;
#endif

enum class ReallocType {
    Uninitialized,
    ZeroInitialized
};

class Allocator {
public:
    uint8_t* Allocate(size_t size) {
        return static_cast<uint8_t*>(std::malloc(size));
    }

    void Free(void* ptr) {
        std::free(ptr);
    }

    uint8_t* Reallocate(void* ptr, size_t newSize, ReallocType reallocType) {
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
    const size_t alignment = PKTALLOC_ALIGN_BYTES;
    return (size + alignment - 1) & ~(alignment - 1);
}

} // namespace pktalloc

#endif // PKTALLOC_H

