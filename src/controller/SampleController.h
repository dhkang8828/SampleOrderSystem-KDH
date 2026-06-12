#pragma once
#include "repository/SampleRepository.h"
#include "model/Sample.h"
#include <string>
#include <vector>

class SampleController {
public:
    explicit SampleController(SampleRepository* sampleRepo);

    std::string registerSample(const std::string& name, double avgProductionTime,
                               double yieldRate, int initialStock);
    std::vector<Sample> listSamples();
    std::optional<Sample> findSample(const std::string& sampleId);

private:
    SampleRepository* sampleRepo_;
};
