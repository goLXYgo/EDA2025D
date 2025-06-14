#include "../inc/OutputGenerator.h"
#include <fstream>
#include <iomanip>

using namespace std;

void writeReport(const vector<Transaction> &transactions, const string &output_file)
{

    int read_no_wait = 0, read_wait = 0;
    int write_no_wait = 0, write_wait = 0;

    uint64_t read_total_cycles = 0;
    uint64_t write_total_cycles = 0;
    uint64_t clock_period_ps = 5000; // 根據你的 VCD 檔案推定的時脈週期
    for (const auto &tx : transactions)
    {
        uint64_t cycles = ((tx.end_time - tx.start_time) + clock_period_ps - 1) / clock_period_ps;

        if (tx.type == TransactionType::READ)
        {
            if (tx.has_wait_state)
                read_wait++;
            else
                read_no_wait++;
            read_total_cycles += cycles;
        }
        else
        {
            if (tx.has_wait_state)
                write_wait++;
            else
                write_no_wait++;
            write_total_cycles += cycles;
        }
    }

    ofstream fout(output_file);
    fout << fixed << setprecision(2);

    fout << "Number of Read Transactions with no wait states: " << read_no_wait << endl;
    fout << "Number of Read Transactions with wait states: " << read_wait << endl;
    fout << "Number of Write Transactions with no wait states: " << write_no_wait << endl;
    fout << "Number of Write Transactions with wait states: " << write_wait << endl;
    double avg_read = (read_no_wait + read_wait) ? (double)read_total_cycles / (read_no_wait + read_wait) : 0.0;
    double avg_write = (write_no_wait + write_wait) ? (double)write_total_cycles / (write_no_wait + write_wait) : 0.0;
    fout << "Average Read Cycle: ";
    if ((read_no_wait + read_wait) > 0)
        fout << avg_read << " cycles" << endl;
    else
        fout << "UNKNOWN" << endl;

    fout << "Average Write Cycle: ";
    if ((write_no_wait + write_wait) > 0)
        fout << avg_write << " cycles" << endl;
    else
        fout << "UNKNOWN" << endl;

    fout << "Bus Utilization: UNKNOWN" << endl;
    fout << "Number of Idle Cycles: UNKNOWN" << endl;
    fout << "Number of Completer: UNKNOWN" << endl;
    fout << "CPU Elapsed Time: UNKNOWN" << endl;

    fout << endl;
    fout << "Number of Transactions with Timeout: UNKNOWN" << endl;
    fout << "Number of Out-of-Range Accesses: UNKNOWN" << endl;
    fout << "Number of Mirrored Transactions: UNKNOWN" << endl;
    fout << "Number of Read-Write Overlap Errors: UNKNOWN" << endl;
    fout << endl;

    fout << "Completer 1 PADDR Connections" << endl;
    fout << "a7: UNKNOWN" << endl;
    fout << "a6: UNKNOWN" << endl;
    fout << "a5: UNKNOWN" << endl;
    fout << "a4: UNKNOWN" << endl;
    fout << "a3: UNKNOWN" << endl;
    fout << "a2: UNKNOWN" << endl;
    fout << "a1: UNKNOWN" << endl;
    fout << "a0: UNKNOWN" << endl;
    fout << endl;

    fout << "Completer 1 PWDATA Connections" << endl;
    fout << "d7: UNKNOWN" << endl;
    fout << "d6: UNKNOWN" << endl;
    fout << "d5: UNKNOWN" << endl;
    fout << "d4: UNKNOWN" << endl;
    fout << "d3: UNKNOWN" << endl;
    fout << "d2: UNKNOWN" << endl;
    fout << "d1: UNKNOWN" << endl;
    fout << "d0: UNKNOWN" << endl;

    fout.close();
}
