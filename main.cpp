#include "inc/VCDParser.h"
#include "inc/TransactionAnalyzer.h"
#include "inc/outputwriter.h"
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
    const auto &symbol_table = parser.getSymbolTable();

    TransactionAnalyzer analyzer(parser.getChanges(), parser.getSymbolTable());

    analyzer.analyze();

    const auto &txs = analyzer.getTransactions();
    cout << "[INFO] Total transactions parsed: " << txs.size() << endl;
    for (const auto &tx : txs)
    {
        cout << (tx.type == TransactionType::READ ? "READ" : "WRITE")
             << " at " << tx.start_time
             << " addr=" << tx.addr
             << " data=" << tx.data
             << " wait=" << (tx.has_wait_state ? "YES" : "NO")
             << endl;
    }

    writeReport(txs, "output.txt");
    return 0;
}
