#include "../inc/VCDStreamer.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool VCDStreamer::load(const string &filename,
                       function<void(const VCDChange &)> on_change_cb)
{
    ifstream fin(filename, ios::binary);
    if (!fin.is_open())
        return false;

    char bom[3] = {0};
    fin.read(bom, 3);
    if (!(static_cast<unsigned char>(bom[0]) == 0xEF &&
          static_cast<unsigned char>(bom[1]) == 0xBB &&
          static_cast<unsigned char>(bom[2]) == 0xBF))
    {
        fin.seekg(0); // rewind if not UTF-8 BOM
    }

    string line;
    bool parsing_changes = false;
    VCDChange current_change;
    uint64_t last_timestamp = 0;

    while (getline(fin, line))
    {
        if (line.empty())
            continue;

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
                int width = 1;
                try
                {
                    width = stoi(tokens[2]);
                }
                catch (...)
                {
                }
                string symbol = tokens[3];
                string name = tokens[4];

                symbol_width[symbol] = width;
                symbol_table[symbol] = SignalInfo{name, type, width};
            }
        }
        else if (line == "$dumpvars")
        {
            // Start of signal changes: initialize at timestamp 0
            parsing_changes = true;
            current_change = VCDChange{0, {}};
            // cout << "[DEBUG] Detected $dumpvars -> start VCD at timestamp #0" << std::endl;
        }
        else if (line[0] == '#' && parsing_changes)
        {
            if (!current_change.changes.empty())
                on_change_cb(current_change);

            current_change = VCDChange{};
            current_change.timestamp = stoull(line.substr(1));
            last_timestamp = current_change.timestamp;
        }
        else if (parsing_changes)
        {
            if (line[0] == 'b')
            {
                size_t space = line.find(' ');
                if (space != string::npos)
                {
                    string raw = line.substr(1, space - 1);
                    string symbol = line.substr(space + 1);
                    int width = symbol_width.count(symbol) ? symbol_width[symbol] : 1;
                    string padded = string(width - raw.size(), '0') + raw;
                    current_change.changes.emplace_back(symbol, padded);
                }
            }
            else if (line.size() >= 2) // scalar
            {
                string value = line.substr(0, 1);
                string symbol = line.substr(1);
                current_change.changes.emplace_back(symbol, value);
            }
        }
    }

    if (!current_change.changes.empty())
    {
        on_change_cb(current_change);
    }

    return true;
}

const unordered_map<string, SignalInfo> &VCDStreamer::getSymbolTable() const
{
    return symbol_table;
}

string VCDStreamer::trim(const string &s)
{
    size_t first = s.find_first_not_of(" \t\n\r");
    size_t last = s.find_last_not_of(" \t\n\r");
    return (first == string::npos) ? "" : s.substr(first, last - first + 1);
}
