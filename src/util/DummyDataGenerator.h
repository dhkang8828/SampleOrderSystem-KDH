#pragma once
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

class DummyDataGenerator {
public:
    // Inserts 3 samples and 6 orders (2 per sample) with randomized values.
    // Pass a non-zero seed for deterministic output (used in tests).
    static void generate(SampleRepository& sampleRepo,
                         OrderRepository& orderRepo,
                         unsigned int seed = 0);

    static constexpr int SAMPLE_COUNT = 3;
    static constexpr int ORDERS_PER_SAMPLE = 2;
};
