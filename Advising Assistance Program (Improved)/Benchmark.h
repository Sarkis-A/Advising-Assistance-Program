#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "HashTable.h"
#include "RedBlackTree.h"

/**
 * @file Benchmark.h
 * @brief Data structures and helpers for timing builds and searches over datasets.
 *
 * This module builds the HashTable and RedBlackTree from an input dataset and
 * measures elapsed times for construction and several search scenarios.
 * No exceptions are thrown for missing files; results are returned with
 * zeroed timings if setup fails upstream.
 */

struct BenchResult {
    // Dataset metadata
    std::string datasetName;
    size_t numCourses = 0;
    size_t numSearchTrials = 0;

    // Timings (milliseconds)
    long long buildMs = 0;       // structure build time
    long long searchHitMs = 0;   // repeated successful lookups
    long long searchMissMs = 0;  // repeated unsuccessful lookups
    long long mixedMs = 0;       // mix of hits/misses (ratio controlled by caller)
    long long rangeMs = 0;       // range/prefix scan timing
};

/**
 * @brief Run HashTable benchmarks over the dataset at filePath.
 * @param filePath      Input dataset path.
 * @param searchTrials  Number of trials for search loops.
 * @param mixedHitRatio Ratio in [0,1] of hits in the mixed search loop (e.g., 0.5).
 * @param rangePrefix   Optional prefix for a range/prefix query (e.g., "CS2").
 */
BenchResult RunHashTableBenchmark(const std::string& filePath,
    size_t searchTrials = 5000,
    double mixedHitRatio = 0.5,
    const std::string& rangePrefix = "CS");

/**
 * @brief Run RedBlackTree benchmarks over the dataset at filePath.
 * @param filePath      Input dataset path.
 * @param searchTrials  Number of trials for search loops.
 * @param mixedHitRatio Ratio in [0,1] of hits in the mixed search loop.
 * @param rangePrefix   Optional prefix for a range/prefix query (e.g., "CS2").
 */
BenchResult RunRBTBenchmark(const std::string& filePath,
    size_t searchTrials = 5000,
    double mixedHitRatio = 0.5,
    const std::string& rangePrefix = "CS");

/**
 * @brief Load only course numbers from a CSV-like dataset file.
 * @return Vector of catalog keys in file order.
 */
std::vector<std::string> LoadCourseNumbersOnly(const std::string& filePath);
