#include "inc/VCDParser.h"
#include "inc/TransactionAnalyzer.h"
#include "inc/OutputWriter.h"
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

    TransactionAnalyzer analyzer(parser.getChanges(), parser.getSymbolTable());

    analyzer.analyze();

    const auto &transactions = analyzer.getTransactions();
    const auto &hazard_report = analyzer.getHazardReport();
    const auto &completer_reports = analyzer.getCompleterReports();

    cout << "[INFO] Total transactions parsed: " << transactions.size() << endl;

    for (const auto &tx : transactions)
    {
        cout << (tx.type == TransactionType::READ ? "READ" : "WRITE")
             << " at " << tx.start_time
             << " addr=" << tx.addr
             << " data=" << tx.data
             << " wait=" << (tx.has_wait_state ? "YES" : "NO")
             << endl;
    }

    // Write the full report (transactions + hazards + completer mapping)
    writeReport(transactions, hazard_report, completer_reports, "output.txt");

    return 0;
}
