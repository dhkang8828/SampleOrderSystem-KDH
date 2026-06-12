#pragma once
#include "model/Sample.h"
#include <vector>
#include <string>

class SampleView {
public:
    void showMenu();
    int getMenuChoice();
    void showSampleList(const std::vector<Sample>& samples);
    void showSample(const Sample& sample);
    void showNotFound(const std::string& id);

    struct SampleInput { std::string name; double avgProdTime; double yieldRate; int stock; };
    SampleInput getSampleInput();
    std::string getSampleId();
};
