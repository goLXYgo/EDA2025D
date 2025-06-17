// =========================
// apb_vcd_analyzer/VCDParser.h
// =========================
#ifndef VCD_PARSER_H
#define VCD_PARSER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cstdint>
#include <fstream>
#include <sstream>

using namespace std;

struct SignalInfo {
    string name;
    string type;
    int width;
};

struct VCDChange {
    uint64_t timestamp;
    vector<pair<string, string>> changes;
};

class VCDParser {
public:
    VCDParser();
    bool load(const string &filename);
    const unordered_map<string, SignalInfo> &getSymbolTable() const;
    const vector<VCDChange> &getChanges() const;

private:
    unordered_map<string, SignalInfo> symbol_table;
    vector<VCDChange> changes;

    string trim(const string &s);
    string binToHex(const string &bin);
};

#endif // VCD_PARSER_H
