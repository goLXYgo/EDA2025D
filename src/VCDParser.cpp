#include "../inc/VCDParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

VCDParser::VCDParser() {}

bool VCDParser::load(const string &filename)
{

    ifstream fin(filename, ios::binary);
    char bom[3] = {0};
    fin.read(bom, 3);
    if (!(static_cast<unsigned char>(bom[0]) == 0xEF && static_cast<unsigned char>(bom[1]) == 0xBB && static_cast<unsigned char>(bom[2]) == 0xBF))
    {
        fin.seekg(0); // not a UTF-8 BOM, rewind
    }
    if (!fin.is_open())
        return false;

    string line;
    bool parsing_changes = false;

    while (getline(fin, line))
    {
        if (line.find("$var") != string::npos)
        {
            istringstream iss(line);
            vector<string> tokens;
            string tok;
            while (iss >> tok)
                tokens.push_back(tok);

            if (tokens.size() >= 5)
            {
                string type = tokens[1];
                string width_str = tokens[2];
                string symbol = tokens[3];
                string name = tokens[4];

                int width = 1;
                try
                {
                    width = stoi(width_str);
                }
                catch (...)
                {
                    width = 1;
                }

                symbol_width[symbol] = width;

                // ✅ 新增這段，讓 TransactionAnalyzer 能找到 psel、penable 等對應 symbol
                SignalInfo info;
                info.name = name;
                info.type = type;
                info.width = width;
                symbol_table[symbol] = info;
            }
        }

        else if (line == "$dumpvars")
        {
            parsing_changes = true;

            // ✅ 建立 timestamp = 0 的初始化變化記錄
            VCDChange init_change;
            init_change.timestamp = 0;
            changes.push_back(init_change);
            continue;
        }
        else if (line == "$enddefinitions $end")
        {
            continue;
        }
        else if (parsing_changes)
        {
            if (line.empty())
                continue;

            if (line[0] == '#')
            {
                VCDChange change;
                change.timestamp = stoull(line.substr(1));
                changes.push_back(change);
            }
            else if (!changes.empty())
            {
                VCDChange &current = changes.back();
                if (line[0] == 'b')
                {
                    // 格式: bxxxx <symbol>
                    size_t space_pos = line.find(' ');
                    if (space_pos != string::npos)
                    {
                        string raw_value = line.substr(1, space_pos - 1); // e.g., "1101..."
                        string symbol = line.substr(space_pos + 1);       // e.g., "%"

                        int width = symbol_width.count(symbol) ? symbol_width[symbol] : 1;
                        string padded_value = string(width - raw_value.size(), '0') + raw_value;

                        signal_value[symbol] = padded_value;

                        // ✅ 這一行不能漏，否則不會被放入 changes
                        current.changes.emplace_back(symbol, padded_value);
                    }
                }

                else
                {
                    // scalar: <value><symbol>
                    string value = line.substr(0, 1);
                    string symbol = line.substr(1);
                    current.changes.emplace_back(symbol, value);
                }
            }
        }
    }
    // cout << "[INFO] Parsed " << changes.size() << " timestamp blocks." << endl;
    for (const auto &change : changes)
    {
        // cout << "#" << change.timestamp << endl;
        for (const auto &[symbol, value] : change.changes)
        {
            string sym_clean = trim(symbol);
            auto it = symbol_table.find(sym_clean);
            string name = (it != symbol_table.end()) ? it->second.name : "??";
            // cout << "  " << name << "(" << sym_clean << ") = " << value << endl;
        }
    }
    return true;
}

const unordered_map<string, SignalInfo> &VCDParser::getSymbolTable() const
{
    return symbol_table;
}

const vector<VCDChange> &VCDParser::getChanges() const
{
    return changes;
}

string VCDParser::trim(const string &s)
{
    size_t first = s.find_first_not_of(" \t\n\r");
    size_t last = s.find_last_not_of(" \t\n\r");
    return (first == string::npos) ? "" : s.substr(first, last - first + 1);
}

string VCDParser::binToHex(const string &bin)
{
    try
    {
        stringstream ss;
        ss << hex << uppercase << stoi(bin, nullptr, 2);
        return ss.str();
    }
    catch (...)
    {
        return "??"; // fallback if binary string is malformed
    }
}
