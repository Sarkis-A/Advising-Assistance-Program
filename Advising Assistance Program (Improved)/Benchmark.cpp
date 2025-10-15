#include "Benchmark.h"
#include "FileLoader.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_set>
#include <vector>
using namespace std;

// Measure time to collect and sort all keys exposed via a callback that visits Courses.
template <typename ForEachFn>
static long long MeasureSortAllKeysMs(ForEachFn forEachFn) {
    using clk = chrono::high_resolution_clock;
    vector<string> keys;
    keys.reserve(1024);

    auto startTime = clk::now();
    forEachFn([&](const Course& c) { keys.push_back(c.number); });
    sort(keys.begin(), keys.end());
    auto endTime = clk::now();
    return chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
}

// Count how many courses start with a given prefix using a callback walker.
template <typename ForEachFn>
static pair<long long, size_t> MeasurePrefixCountMs(ForEachFn forEachFn, const string& prefix) {
    using clk = chrono::high_resolution_clock;
    auto startTime = clk::now();
    size_t count = 0;
    forEachFn([&](const Course& c) {
        if (c.number.rfind(prefix, 0) == 0) ++count;
        });
    auto endTime = clk::now();
    return { chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count(), count };
}

// Utility: load only the first CSV field (course number) from the file.
vector<string> LoadCourseNumbersOnly(const string& filePath) {
    vector<string> out;
    ifstream file(filePath);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string number;
        if (getline(ss, number, ',')) {
            if (!number.empty()) out.push_back(number);
        }
    }
    return out;
}

// --- HashTable benchmark ------------------------------------------------------
BenchResult RunHashTableBenchmark(const string& filePath,
    size_t searchTrials,
    double mixedHitRatio,
    const string& rangePrefix) {
    BenchResult result{};
    result.datasetName = filePath;

    // Build HashTable by streaming the file with FileLoader.
    {
        HashTable hashTable(10007); // initial capacity
        auto startTime = chrono::high_resolution_clock::now();
        loadCourses(hashTable, filePath);
        auto endTime = chrono::high_resolution_clock::now();
        result.buildMs = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

        // Determine dataset size via iteration; avoids exposing internals.
        size_t courseCount = 0;
        hashTable.ForEach([&](const Course&) { ++courseCount; });
        result.numCourses = courseCount;

        // Prepare search key pools (hits from dataset; misses synthetic).
        vector<string> hitKeys = LoadCourseNumbersOnly(filePath);
        if (!hitKeys.empty() && hitKeys.size() > searchTrials) {
            hitKeys.resize(searchTrials);
        }
        vector<string> missKeys;
        missKeys.reserve(hitKeys.size());
        for (size_t i = 0; i < hitKeys.size(); ++i) {
            missKeys.push_back(hitKeys[i] + "_MISS"); // guaranteed miss without altering the table
        }

        result.numSearchTrials = searchTrials;

        // Time repeated successful lookups
        {
            size_t poolSize = hitKeys.empty() ? 1 : hitKeys.size();
            mt19937 rngHit(12345);
            uniform_int_distribution<size_t> hitIndexDist(0, poolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                const string& key = hitKeys[hitIndexDist(rngHit)];
                volatile Course sink = hashTable.Search(key); // volatile prevents optimization away
                (void)sink;
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.searchHitMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Time repeated unsuccessful lookups
        {
            size_t poolSize = missKeys.empty() ? 1 : missKeys.size();
            mt19937 rngMiss(67890);
            uniform_int_distribution<size_t> missIndexDist(0, poolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                const string& key = missKeys[missIndexDist(rngMiss)];
                volatile Course sink = hashTable.Search(key);
                (void)sink;
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.searchMissMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Mixed loop (hits and misses by ratio)
        {
            size_t hitPoolSize = hitKeys.empty() ? 1 : hitKeys.size();
            size_t missPoolSize = missKeys.empty() ? 1 : missKeys.size();

            mt19937 rngMixed(13579);
            uniform_real_distribution<double> coinFlip(0.0, 1.0);
            uniform_int_distribution<size_t> hitIndexDist(0, hitPoolSize - 1);
            uniform_int_distribution<size_t> missIndexDist(0, missPoolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                if (coinFlip(rngMixed) < mixedHitRatio) {
                    const string& key = hitKeys[hitIndexDist(rngMixed)];
                    volatile Course sink = hashTable.Search(key);
                    (void)sink;
                }
                else {
                    const string& key = missKeys[missIndexDist(rngMixed)];
                    volatile Course sink = hashTable.Search(key);
                    (void)sink;
                }
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.mixedMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Prefix count and key collection timings
        result.rangeMs = MeasurePrefixCountMs([&](auto&& fn) { hashTable.ForEach(fn); }, rangePrefix).first;
    }

    return result;
}

// --- RedBlackTree benchmark ---------------------------------------------------
BenchResult RunRBTBenchmark(const string& filePath,
    size_t searchTrials,
    double mixedHitRatio,
    const string& rangePrefix) {
    BenchResult result{};
    result.datasetName = filePath;

    {
        RedBlackTree rbt;

        // Build time: stream file and insert Courses
        auto startTime = chrono::high_resolution_clock::now();
        loadCourses(rbt, filePath);
        auto endTime = chrono::high_resolution_clock::now();
        result.buildMs = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

        // Determine dataset size by traversal
        result.numCourses = rbt.Size();

        // Prepare search key pools
        vector<string> hitKeys = LoadCourseNumbersOnly(filePath);
        if (!hitKeys.empty() && hitKeys.size() > searchTrials) {
            hitKeys.resize(searchTrials);
        }
        vector<string> missKeys;
        missKeys.reserve(hitKeys.size());
        for (size_t i = 0; i < hitKeys.size(); ++i) {
            missKeys.push_back(hitKeys[i] + "_MISS");
        }

        result.numSearchTrials = searchTrials;

        // Successful lookups
        {
            size_t poolSize = hitKeys.empty() ? 1 : hitKeys.size();
            mt19937 rngHit(24680);
            uniform_int_distribution<size_t> hitIndexDist(0, poolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                const string& key = hitKeys[hitIndexDist(rngHit)];
                volatile Course sink = rbt.Search(key);
                (void)sink;
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.searchHitMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Unsuccessful lookups
        {
            size_t poolSize = missKeys.empty() ? 1 : missKeys.size();
            mt19937 rngMiss(97531);
            uniform_int_distribution<size_t> missIndexDist(0, poolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                const string& key = missKeys[missIndexDist(rngMiss)];
                volatile Course sink = rbt.Search(key);
                (void)sink;
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.searchMissMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Mixed lookups
        {
            size_t hitPoolSize = hitKeys.empty() ? 1 : hitKeys.size();
            size_t missPoolSize = missKeys.empty() ? 1 : missKeys.size();

            mt19937 rngMixed(11223);
            uniform_real_distribution<double> coinFlip(0.0, 1.0);
            uniform_int_distribution<size_t> hitIndexDist(0, hitPoolSize - 1);
            uniform_int_distribution<size_t> missIndexDist(0, missPoolSize - 1);

            auto loopStart = chrono::high_resolution_clock::now();
            for (size_t i = 0; i < searchTrials; ++i) {
                if (coinFlip(rngMixed) < mixedHitRatio) {
                    const string& key = hitKeys[hitIndexDist(rngMixed)];
                    volatile Course sink = rbt.Search(key);
                    (void)sink;
                }
                else {
                    const string& key = missKeys[missIndexDist(rngMixed)];
                    volatile Course sink = rbt.Search(key);
                    (void)sink;
                }
            }
            auto loopEnd = chrono::high_resolution_clock::now();
            result.mixedMs = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart).count();
        }

        // Prefix count and key collection timings
        result.rangeMs = MeasurePrefixCountMs([&](auto&& fn) { rbt.ForEach(fn); }, rangePrefix).first;
    }

    return result;
}
