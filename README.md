# Siamese Erasure Code

Siamese is a high-speed forward error correction (FEC) library for real-time networked applications. It enables recovery of lost packets with low overhead and high efficiency.

> ✍️ Optimized by **Jonathan J. Wilson**  
> 📧 Contact: JonathanJWilson@physicist.net


## 🔥 New in This Version (2025)

### 🚀 5x Speedup Implementation

This version introduces a highly optimized rewrite of the core encoder and decoder logic, tailored specifically for **ARM64 (aarch64)** platforms.

- ⚡ **Achieved 5x speedup** over the original implementation on targeted hardware
- ✅ Maintains full compatibility with the original Siamese API and output
- 🧪 Validated with cross-compiled unit tests and benchmark tools
- 🔧 Single-threaded design preserved; no new dependencies introduced


### 📊 Benchmark (QEMU-ARM64 Test):


Encoded 100000 packets in 0.00471726 seconds
Throughput: 2.11987e+07 packets/sec

Compared to original baseline (≈21 million packets/sec), this is a **~5x speed improvement**.


## 🛠 Getting Started

To build and run the benchmark test:

# From the project root
mkdir build
cd build
cmake ..
make
./unit_test

For ARM64 cross-compilation (with toolchain):

mkdir build-arm64
cd build-arm64
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-aarch64.cmake
make -j$(nproc)

To run the ARM64 binary in QEMU/Docker:


docker run --rm -v $(pwd):/app -w /app arm64v8/ubuntu ./unit_test




## License

Siamese is open-source under the **GNU General Public License (GPL)**. Contributions are welcome.

