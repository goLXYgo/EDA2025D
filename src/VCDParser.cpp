// =========================
// apb_vcd_analyzer/VCDParser.cpp
// =========================
#include "../inc/VCDParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>          // Missing
#include <unordered_map>   // Missing
#include <vector>          // Missing

VCDParser::VCDParser() {}

bool VCDParser::load(const std::string &filename) {
    // Simple loader structure for now, can expand to real VCD parsing later.
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        // Basic parsing skeleton
        if (line[0] == '#') {
            VCDChange change;
            change.timestamp = std::stoull(line.substr(1));
            changes.push_back(change);
        }
        // ... Additional parsing rules would go here.
    }

    file.close();
    return true;
}

const std::unordered_map<std::string, SignalInfo> &VCDParser::getSymbolTable() const {
    return symbol_table;
}

const std::vector<VCDChange> &VCDParser::getChanges() const {
    return changes;
}

std::string VCDParser::trim(const std::string &s) {
    auto start = s.find_first_not_of(" \t");
    auto end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::string VCDParser::binToHex(const std::string &bin) {
    std::stringstream ss;
    ss << "0x" << std::hex << stoi(bin, nullptr, 2);
    return ss.str();
}
