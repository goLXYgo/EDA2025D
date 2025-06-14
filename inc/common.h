#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct SignalInfo
{
    std::string name;
    std::string type;
    int width;
};

struct VCDChange
{
    uint64_t timestamp;
    std::vector<std::pair<std::string, std::string>> changes;
};
