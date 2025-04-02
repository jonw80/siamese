#ifndef PKTALLOC_H
#define PKTALLOC_H

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace pktalloc {

enum class Realloc {
    Uninitialized,
    ZeroInitialized
};

class Allocator {
public:
    void* Allocate(size_t size) {
        return std::malloc(size);
    }

    void Free(void* ptr) {
        std::free(ptr);
    }

    void* Reallocate(void* ptr, size_t newSize, Realloc reallocType) {
        if (reallocType == Realloc::ZeroInitialized) {
            void* newPtr = std::calloc(1, newSize);
            if (ptr) {
                std::memcpy(newPtr, ptr, newSize);
                std::free(ptr);
            }
            return newPtr;
        }
        return std::realloc(ptr, newSize);
    }
};

inline size_t NextAlignedOffset(size_t size) {
    const size_t alignment = 16; // Example alignment
    return (size + alignment - 1) & ~(alignment - 1);
}

} // namespace pktalloc

#endif // PKTALLOC_H
