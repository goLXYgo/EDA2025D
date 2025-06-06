#include "VCDParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

VCDParser::VCDParser() {} // 預設建構子

bool VCDParser::load(const string &filename)
{
    ifstream fin(filename); // 開啟檔案
    if (!fin.is_open())
        return false; // 若無法開啟則回傳 false

    string line;
    bool in_definitions = true;
    uint64_t current_time = 0;

    while (getline(fin, line))
    {
        line = trim(line); // 去除空白

        if (line.find("$var") == 0)
        { // 處理訊號定義行
            istringstream iss(line);
            string temp, type, symbol, name;
            int width;

            iss >> temp >> type >> width >> symbol >> name;
            symbol_table[symbol] = {name, type, width}; // 儲存 symbol 對應
        }

        if (line == "$enddefinitions $end")
            break; // 結束宣告區則跳出
    }

    while (getline(fin, line))
    {
        line = trim(line); // 處理 $dumpvars 區段
        if (line == "$dumpvars")
        {
            while (getline(fin, line))
            {
                line = trim(line);
                if (line == "$end")
                    break; // 結束 dumpvars 區段
            }
            break;
        }
    }

    while (getline(fin, line))
    {
        line = trim(line); // 處理訊號變化
        if (line.empty())
            continue;

        if (line[0] == '#')
        { // 處理時間戳記
            current_time = stoull(line.substr(1));
            changes.push_back({current_time, {}}); // 建立新事件
        }
        else if (line[0] == 'b')
        { // 多位元變化
            size_t space_pos = line.find(' ');
            string bin_value = line.substr(1, space_pos - 1);
            string symbol = line.substr(space_pos + 1);
            if (symbol_table.count(symbol))
            {
                string signal_name = symbol_table[symbol].name;
                string hex_value = binToHex(bin_value);                     // 轉為 hex
                changes.back().changes.push_back({signal_name, hex_value}); // 記錄變化
            }
        }
        else
        { // 單位元變化
            char value = line[0];
            string symbol = line.substr(1);
            if (symbol_table.count(symbol))
            {
                string signal_name = symbol_table[symbol].name;
                changes.back().changes.push_back({signal_name, string(1, value)}); // 記錄變化
            }
        }
    }

    return true; // 成功載入
}

const unordered_map<string, SignalInfo> &VCDParser::getSymbolTable() const
{
    return symbol_table; // 回傳符號表
}

const vector<VCDChange> &VCDParser::getChanges() const
{
    return changes; // 回傳訊號變化事件
}

string VCDParser::trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1); // 去除首尾空白
}

string VCDParser::binToHex(const string &bin)
{
    stringstream ss;
    unsigned long val = stoul(bin, nullptr, 2); // 二進位轉整數
    ss << "0x" << hex << uppercase << val;      // 輸出為十六進位字串
    return ss.str();
}
