#include "VCDParser.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./vcdtest <vcd_file>" << endl;
        return 1;
    }

    string filename = argv[1];
    VCDParser parser;

    if (!parser.load(filename))
    {
        cerr << "Failed to load VCD file: " << filename << endl;
        return 1;
    }

    const auto &changes = parser.getChanges();
    for (const auto &change : changes)
    {
        cout << "#" << change.timestamp << endl;
        for (const auto &signal_change : change.changes)
        {
            cout << "  " << signal_change.first << " = " << signal_change.second << endl;
        }
    }

    return 0;
}
