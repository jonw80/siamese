cmake_minimum_required(VERSION 3.5)
project(siamese LANGUAGES C CXX)


################################################################################
# Configuration

# Verbose compilation?
#set(CMAKE_VERBOSE_MAKEFILE ON)


################################################################################
# Build Settings

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# In debug mode, add -DDEBUG
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    # Warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

    # Remove Asio warning
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")

    # Static library: -fPIC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # Gprof Profiling (optional)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

    # Architecture-specific flags
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -march=armv8-a -fstack-protector")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=armv8-a")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -march=native -fstack-protector")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
    endif()
endif()

#include_directories(include)


################################################################################
# Dependencies

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


################################################################################
# Source Files

set(INCLUDE_FILES
    Logger.h
)

set(SOURCE_FILES
    ${INCLUDE_FILES}
    gf256.cpp
    gf256_arm64.s
    Logger.cpp
    PacketAllocator.cpp
    SiameseCommon.cpp
    siamese.cpp
    SiameseDecoder.cpp
    SiameseEncoder.cpp
    SiameseTools.cpp
    #tests/gentab_primes.cpp
    #tests/GF256Matrix.cpp
    #tests/GF256Matrix.h
    #tests/test_allocator.cpp
    #tests/test_custom_bitset.cpp
    #tests/test_invert_rate.cpp
    #tests/test_recovery_sort.cpp
    #tests/test_serializers.cpp
    tests/TestTools.cpp
    tests/TestTools.h
    tests/unit_test.cpp
)


################################################################################
# Targets

# Application

set(SOURCES
    siamese.cpp
    SiameseEncoder.cpp
    SiameseDecoder.cpp
    Logger.cpp
    logger.cpp
    # Add other source files as needed
)

add_executable(unit_test
    siamese.cpp
    SiameseEncoder.cpp
    SiameseDecoder.cpp
    Logger.cpp
    logger.cpp
    unit_test/main.cpp
)
target_include_directories(unit_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(unit_test Threads::Threads)
install(TARGETS unit_test DESTINATION bin)

# Ensure Logger.h is included in the build process
add_library(siamese STATIC ${SOURCES})
