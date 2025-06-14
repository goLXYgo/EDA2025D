#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "Common.h" // ✅ 引入 VCDChange 和 SignalInfo

class VCDStreamer
{
public:
    bool load(const std::string &filename,
              std::function<void(const VCDChange &)> on_change_cb);

    const std::unordered_map<std::string, SignalInfo> &getSymbolTable() const;

private:
    std::unordered_map<std::string, SignalInfo> symbol_table;
    std::unordered_map<std::string, int> symbol_width;

    std::string trim(const std::string &s);
};
