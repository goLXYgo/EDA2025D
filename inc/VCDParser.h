#ifndef VCD_PARSER_H
#define VCD_PARSER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cstdint>

struct SignalInfo {
    std::string name;
    std::string type;
    int width;
};

struct VCDChange {
    uint64_t timestamp;
    std::vector<std::pair<std::string, std::string>> changes;
};

class VCDParser {
public:
    VCDParser();
    bool load(const std::string &filename);
    const std::unordered_map<std::string, SignalInfo> &getSymbolTable() const;
    const std::vector<VCDChange> &getChanges() const;

private:
    std::unordered_map<std::string, SignalInfo> symbol_table;
    std::vector<VCDChange> changes;

    std::string trim(const std::string &s);
    std::string binToHex(const std::string &bin);
};

#endif // VCD_PARSER_H
