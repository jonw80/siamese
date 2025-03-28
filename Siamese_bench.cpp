/** \file
    \brief Siamese Benchmarks
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

#include "siamese.h"
#include "SiameseTools.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>

static const unsigned kBenchmarkRuns = 10;
static const unsigned kBenchmarkPackets = 32;
static const unsigned kBenchmarkPacketSize = 1024;

struct BenchmarkResult
{
    uint64_t AddTime;
    uint64_t EncodeTime;
    uint64_t DecodeTime;

    BenchmarkResult()
    {
        AddTime = 0;
        EncodeTime = 0;
        DecodeTime = 0;
    }
};

static BenchmarkResult RunOneBenchmarkAllocs()
{
    BenchmarkResult result;

    // Allocate the encoder
    SiameseEncoder encoder = nullptr;
    SiameseResult r = siamese_encoder_create(&encoder);
    if (r != Siamese_Success || !encoder)
    {
        std::cout << "Error: Unable to create encoder r=" << r << std::endl;
        return result;
    }

    // Allocate the decoder
    SiameseDecoder decoder = nullptr;
    r = siamese_decoder_create(&decoder);
    if (r != Siamese_Success || !decoder)
    {
        std::cout << "Error: Unable to create decoder r=" << r << std::endl;
        siamese_encoder_destroy(encoder);
        return result;
    }

    // Generating packet data
    std::vector<uint8_t> packetData;
    packetData.resize(kBenchmarkPacketSize);
    
    for (size_t i = 0; i < packetData.size(); ++i)
        packetData[i] = static_cast<uint8_t>(i);

    // Prepare benchmark
    std::vector<SiameseOriginalPacket> originalPackets(kBenchmarkPackets);
    
    for (unsigned i = 0; i < kBenchmarkPackets; ++i)
    {
        originalPackets[i].Data = packetData.data();
        originalPackets[i].DataBytes = kBenchmarkPacketSize;
        originalPackets[i].PacketNum = 0; // will be set by encoder
    }

    // Prepare recovery packet
    SiameseRecoveryPacket recoveryPacket;
    std::vector<uint8_t> recoveryBuffer;
    recoveryBuffer.resize(kBenchmarkPacketSize * 24);
    recoveryPacket.Data = recoveryBuffer.data();
    recoveryPacket.AllocatedBytes = static_cast<unsigned>(recoveryBuffer.size());
    recoveryPacket.DataBytes = 0;
    recoveryPacket.PacketNum = 0;

    // Timing variables
    uint64_t addStart, addEnd, encodeStart, encodeEnd, decodeStart, decodeEnd;

    // Benchmark encoding
    addStart = siamese::GetTimeUsec();

    for (unsigned i = 0; i < kBenchmarkPackets; ++i)
    {
        r = siamese_encoder_add(encoder, &originalPackets[i]);
        if (r != Siamese_Success)
        {
            std::cout << "Error: Unable to add original packet r=" << r << std::endl;
            siamese_encoder_destroy(encoder);
            siamese_decoder_destroy(decoder);
            return result;
        }
    }

    addEnd = siamese::GetTimeUsec();
    result.AddTime = addEnd - addStart;

    // Measure time to generate a recovery packet
    encodeStart = siamese::GetTimeUsec();
    
    r = siamese_encode(encoder, &recoveryPacket);
    if (r != Siamese_Success)
    {
        std::cout << "Error: Unable to encode recovery packet r=" << r << std::endl;
        siamese_encoder_destroy(encoder);
        siamese_decoder_destroy(decoder);
        return result;
    }
    
    encodeEnd = siamese::GetTimeUsec();
    result.EncodeTime = encodeEnd - encodeStart;

    // Measure time to decode the packets
    decodeStart = siamese::GetTimeUsec();
    
    r = siamese_decode(decoder, &recoveryPacket);
    if (r != Siamese_Success && r != Siamese_NeedMoreData)
    {
        std::cout << "Error: Unable to decode recovery packet r=" << r << std::endl;
        siamese_encoder_destroy(encoder);
        siamese_decoder_destroy(decoder);
        return result;
    }
    
    decodeEnd = siamese::GetTimeUsec();
    result.DecodeTime = decodeEnd - decodeStart;

    // Clean up
    siamese_encoder_destroy(encoder);
    siamese_decoder_destroy(decoder);

    return result;
}

static void PrintBenchmarkResults(const std::vector<BenchmarkResult>& results)
{
    uint64_t sumAddTime = 0, sumEncodeTime = 0, sumDecodeTime = 0;
    uint64_t minAddTime = UINT64_MAX, minEncodeTime = UINT64_MAX, minDecodeTime = UINT64_MAX;
    uint64_t maxAddTime = 0, maxEncodeTime = 0, maxDecodeTime = 0;

    for (const auto& result : results)
    {
        sumAddTime += result.AddTime;
        sumEncodeTime += result.EncodeTime;
        sumDecodeTime += result.DecodeTime;

        minAddTime = std::min(minAddTime, result.AddTime);
        minEncodeTime = std::min(minEncodeTime, result.EncodeTime);
        minDecodeTime = std::min(minDecodeTime, result.DecodeTime);

        maxAddTime = std::max(maxAddTime, result.AddTime);
        maxEncodeTime = std::max(maxEncodeTime, result.EncodeTime);
        maxDecodeTime = std::max(maxDecodeTime, result.DecodeTime);
    }

    double avgAddTime = static_cast<double>(sumAddTime) / results.size();
    double avgEncodeTime = static_cast<double>(sumEncodeTime) / results.size();
    double avgDecodeTime = static_cast<double>(sumDecodeTime) / results.size();

    double stddevAddTime = 0, stddevEncodeTime = 0, stddevDecodeTime = 0;

    for (const auto& result : results)
    {
        double diff = static_cast<double>(result.AddTime) - avgAddTime;
        stddevAddTime += diff * diff;

        diff = static_cast<double>(result.EncodeTime) - avgEncodeTime;
        stddevEncodeTime += diff * diff;

        diff = static_cast<double>(result.DecodeTime) - avgDecodeTime;
        stddevDecodeTime += diff * diff;
    }

    stddevAddTime = std::sqrt(stddevAddTime / results.size());
    stddevEncodeTime = std::sqrt(stddevEncodeTime / results.size());
    stddevDecodeTime = std::sqrt(stddevDecodeTime / results.size());

    std::cout << "Benchmark Results (" << kBenchmarkPackets << " packets of " 
              << kBenchmarkPacketSize << " bytes each):" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << "Add Operation:     " << avgAddTime / 1000.0 << " ms average (" 
              << stddevAddTime / 1000.0 << " ms stddev), " 
              << "Range: [" << minAddTime / 1000.0 << ", " << maxAddTime / 1000.0 << "]" << std::endl;
              
    std::cout << "Encode Operation:  " << avgEncodeTime / 1000.0 << " ms average (" 
              << stddevEncodeTime / 1000.0 << " ms stddev), " 
              << "Range: [" << minEncodeTime / 1000.0 << ", " << maxEncodeTime / 1000.0 << "]" << std::endl;
              
    std::cout << "Decode Operation:  " << avgDecodeTime / 1000.0 << " ms average (" 
              << stddevDecodeTime / 1000.0 << " ms stddev), " 
              << "Range: [" << minDecodeTime / 1000.0 << ", " << maxDecodeTime / 1000.0 << "]" << std::endl;

    double throughputMBps = (kBenchmarkPackets * kBenchmarkPacketSize) / (avgAddTime + avgEncodeTime);
    std::cout << "Encoder Throughput: " << throughputMBps << " MB/s" << std::endl;

    throughputMBps = (kBenchmarkPackets * kBenchmarkPacketSize) / avgDecodeTime;
    std::cout << "Decoder Throughput: " << throughputMBps << " MB/s" << std::endl;
}

int main(int argc, char* argv[])
{
    // Initialize Siamese
    SiameseResult r = siamese_init();
    if (r != Siamese_Success)
    {
        std::cout << "Error: Unable to initialize Siamese library r=" << r << std::endl;
        return -1;
    }

    std::cout << "Starting Siamese benchmark..." << std::endl;
    std::cout << "Running " << kBenchmarkRuns << " iterations..." << std::endl;

    std::vector<BenchmarkResult> results;
    results.reserve(kBenchmarkRuns);

    for (unsigned i = 0; i < kBenchmarkRuns; ++i)
    {
        std::cout << "Running benchmark iteration " << (i+1) << "..." << std::endl;
        BenchmarkResult result = RunOneBenchmarkAllocs();
        results.push_back(result);
    }

    PrintBenchmarkResults(results);

    return 0;
}