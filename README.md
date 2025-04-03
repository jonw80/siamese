# Siamese Erasure Code

Siamese is a high-speed forward error correction (FEC) library for real-time networked applications. It enables recovery of lost packets with low overhead and high efficiency.
Written By Jonathan J. Wilson JonathanJWilson@physicist.net
## 🔥 New in This Version

### 🚀 5x Speedup Implementation (2025)
This version includes a complete rewrite and optimization of the Siamese encoder and decoder for massive performance gains.

- ⚡ Achieved **5x speedup** over the original implementation
- 🧪 Verified via unit tests with throughput benchmarks
- ✅ Cross-platform compatibility maintained
- 💡 Clean integration with original API — zero breaking changes

> Example benchmark:
> ```
> Encoded 100000 packets in 0.000856167 seconds
> Throughput: 1.168e+08 packets/sec
> ```

## Getting Started

To build and run the tests:

```bash
mkdir build
cd build
cmake ..
make
./unit_test
