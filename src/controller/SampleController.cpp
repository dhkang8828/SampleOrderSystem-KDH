#include "controller/SampleController.h"
#include "util/IdGenerator.h"

SampleController::SampleController(SampleRepository* sampleRepo)
    : sampleRepo_(sampleRepo) {}

std::string SampleController::registerSample(const std::string& name,
                                              double avgProductionTime,
                                              double yieldRate,
                                              int initialStock) {
    int seq = sampleRepo_->getNextSequence();
    std::string sampleId = IdGenerator::generateSampleId(seq);
    Sample sample(sampleId, name, avgProductionTime, yieldRate, initialStock);
    sampleRepo_->save(sample);
    return sampleId;
}

std::vector<Sample> SampleController::listSamples() {
    return sampleRepo_->findAll();
}

std::optional<Sample> SampleController::findSample(const std::string& sampleId) {
    return sampleRepo_->findById(sampleId);
}
