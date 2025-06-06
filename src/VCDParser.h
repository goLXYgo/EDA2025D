#ifndef VCD_PARSER_H
#define VCD_PARSER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cstdint>

using namespace std;

struct SignalInfo
{
    string name; // 如 "paddr"
    string type; // 如 "wire"、"reg"、"parameter"
    int width;   // 如 32
};

struct VCDChange
{
    uint64_t timestamp;                   // 如 #200000
    vector<pair<string, string>> changes; // e.g., { ("paddr", "0x1F"), ("pwrite", "1") }
};

class VCDParser
{
public:
    VCDParser();
    bool load(const string &filename); // 載入並解析 VCD 檔案

    const unordered_map<string, SignalInfo> &getSymbolTable() const; // 取得符號對應（如 '%' → paddr）
    const vector<VCDChange> &getChanges() const;                     // 取得解析後的訊號變化事件序列

private:
    unordered_map<string, SignalInfo> symbol_table; // 儲存符號對應表
    vector<VCDChange> changes;                      // 儲存所有時間點的訊號變化

    string trim(const string &s);       // 移除字串前後空白
    string binToHex(const string &bin); // 將二進位字串轉為十六進位表示
};

#endif // VCD_PARSER_H
