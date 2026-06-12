#include "util/DummyDataGenerator.h"
#include "util/IdGenerator.h"
#include "util/DateTimeUtil.h"
#include <random>
#include <array>

namespace {

struct SampleTemplate {
    const char* name;
};

constexpr std::array<SampleTemplate, 5> SAMPLE_POOL = {{
    {"AlphaX"},
    {"BetaY"},
    {"GammaZ"},
    {"DeltaW"},
    {"EpsilonV"},
}};

constexpr std::array<const char*, 6> CUSTOMER_POOL = {{
    "CorpAlpha",
    "CorpBeta",
    "CorpGamma",
    "CorpDelta",
    "CorpEpsilon",
    "CorpZeta",
}};

} // namespace

void DummyDataGenerator::generate(SampleRepository& sampleRepo,
                                  OrderRepository& orderRepo,
                                  unsigned int seed) {
    std::mt19937 rng(seed != 0 ? seed : std::random_device{}());

    // Shuffle indices to pick SAMPLE_COUNT distinct samples from the pool
    std::array<int, 5> idx = {0, 1, 2, 3, 4};
    std::shuffle(idx.begin(), idx.end(), rng);

    std::uniform_real_distribution<double> timeDist(5.0, 30.0);
    std::uniform_real_distribution<double> yieldDist(0.85, 0.99);
    std::uniform_int_distribution<int>    stockDist(30, 200);
    std::uniform_int_distribution<int>    qtyDist(10, 100);
    std::uniform_int_distribution<int>    custDist(0, static_cast<int>(CUSTOMER_POOL.size()) - 1);

    std::string today = DateTimeUtil::today();
    std::string now   = DateTimeUtil::now();

    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        const auto& tmpl = SAMPLE_POOL[idx[i]];
        int seq = sampleRepo.getNextSequence();
        std::string sampleId = IdGenerator::generateSampleId(seq);

        Sample sample(sampleId, tmpl.name,
                      timeDist(rng),
                      yieldDist(rng),
                      stockDist(rng));
        sampleRepo.save(sample);

        for (int j = 0; j < ORDERS_PER_SAMPLE; ++j) {
            int orderSeq = orderRepo.generateNextOrderSequence(today);
            std::string orderId = IdGenerator::generateOrderId(today, orderSeq);
            std::string customer = CUSTOMER_POOL[custDist(rng)];

            Order order(orderId, sampleId, customer, qtyDist(rng), now);
            orderRepo.save(order);
        }
    }
}
