// =========================
// apb_vcd_analyzer/VCDParser.cpp
// =========================
#include "../inc/VCDParser.h"
#include <iostream>

VCDParser::VCDParser() {}

bool VCDParser::load(const string &filename) {
    // Simple loader structure for now, can expand to real VCD parsing later.
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        // Basic parsing skeleton
        if (line[0] == '#') {
            VCDChange change;
            change.timestamp = stoull(line.substr(1));
            changes.push_back(change);
        }
        // ... Additional parsing rules would go here.
    }

    file.close();
    return true;
}

const unordered_map<string, SignalInfo> &VCDParser::getSymbolTable() const {
    return symbol_table;
}

const vector<VCDChange> &VCDParser::getChanges() const {
    return changes;
}

string VCDParser::trim(const string &s) {
    auto start = s.find_first_not_of(" \t");
    auto end = s.find_last_not_of(" \t");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

string VCDParser::binToHex(const string &bin) {
    stringstream ss;
    ss << "0x" << hex << stoi(bin, nullptr, 2);
    return ss.str();
}
