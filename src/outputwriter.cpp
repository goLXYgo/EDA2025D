#include "OutputWriter.h"
#include <fstream>
#include <iostream>

using namespace std;

void writeReport(const std::vector<Transaction> &transactions,
                 const HazardReport &hazard_report,
                 const std::unordered_map<int, CompleterConnectionReport> &completer_reports,
                 const std::string &filename)
{
    ofstream ofs(filename);
    if (!ofs)
    {
        cerr << "Failed to open output file: " << filename << endl;
        return;
    }

    // Example: Writing transaction counts
    int read_no_wait = 0, read_wait = 0, write_no_wait = 0, write_wait = 0;

    for (const auto &tx : transactions)
    {
        if (tx.type == TransactionType::READ)
        {
            if (tx.has_wait_state)
                read_wait++;
            else
                read_no_wait++;
        }
        else
        {
            if (tx.has_wait_state)
                write_wait++;
            else
                write_no_wait++;
        }
    }

    ofs << "Number of Read Transactions with no wait states: " << read_no_wait << endl;
    ofs << "Number of Read Transactions with wait states: " << read_wait << endl;
    ofs << "Number of Write Transactions with no wait states: " << write_no_wait << endl;
    ofs << "Number of Write Transactions with wait states: " << write_wait << endl;

    // Example: Writing some hazard summary
    ofs << "Number of Transactions with Timeout: " << hazard_report.timeout_count << endl;
    ofs << "Number of Out-of-Range Accesses: " << hazard_report.out_of_range_count << endl;
    ofs << "Number of Mirrored Transactions: " << hazard_report.mirrored_count << endl;
    ofs << "Number of Read-Write Overlap Errors: " << hazard_report.overlap_count << endl;

    // Completer connection reports
    for (const auto &entry : completer_reports)
    {
        int completer_id = entry.first;
        const auto &report = entry.second;

        ofs << endl;
        ofs << "Completer " << completer_id << " PADDR Connections" << endl;
        for (const auto &pair : report.paddr_connections)
        {
            ofs << pair.first << ": " << pair.second << endl;
        }

        ofs << endl;
        ofs << "Completer " << completer_id << " PWDATA Connections" << endl;
        for (const auto &pair : report.pwdata_connections)
        {
            ofs << pair.first << ": " << pair.second << endl;
        }
    }

    ofs.close();
    cout << "[INFO] Report written to " << filename << endl;
}
