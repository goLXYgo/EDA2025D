#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include "../inc/VCDStreamer.h"
#include "../inc/TransactionAnalyzer.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: ./APB_Recognizer <input.vcd> <output.txt>" << endl;
        return 1;
    }

    string input_file = argv[1];
    string output_file = argv[2];

    auto start_time = high_resolution_clock::now();

    VCDStreamer streamer;
    TransactionAnalyzer analyzer;

    if (!streamer.load(input_file, [&](const VCDChange &change)
                       { analyzer.feed(change); }))
    {
        cerr << "Failed to load or parse VCD file: " << input_file << endl;
        return 2;
    }

    auto end_time = high_resolution_clock::now();
    double elapsed_ms = duration_cast<microseconds>(end_time - start_time).count() / 1000.0;

    const auto &transactions = analyzer.getTransactions();

    int read_no_wait = 0, read_wait = 0;
    int write_no_wait = 0, write_wait = 0;
    int timeout = 0, total_transaction_cycles = 0, idle_cycles = 0;
    int read_total_cycles = 0, write_total_cycles = 0;
    int read_count = 0, write_count = 0;
    int out_of_range_count = 0, mirrored_count = 0, overlap_error_count = 0;
    uint64_t last_end_time = 0;

    for (const auto &tx : transactions)
    {
        if (tx.timed_out)
        {
            timeout++;
            continue;
        }

        int cycles = (tx.end_time - tx.start_time) / 10000;
        total_transaction_cycles += cycles;

        if (tx.type == TransactionType::READ)
        {
            read_count++;
            read_total_cycles += cycles;
            (tx.has_wait_state ? read_wait : read_no_wait)++;
        }
        else
        {
            write_count++;
            write_total_cycles += cycles;
            (tx.has_wait_state ? write_wait : write_no_wait)++;
        }

        if (tx.out_of_range)
            out_of_range_count++;

        if (tx.data_mirrored)
            mirrored_count++;

        if (tx.read_write_overlap)
            overlap_error_count++;

        if (last_end_time > 0 && tx.start_time > last_end_time)
        {
            idle_cycles += (tx.start_time - last_end_time) / 10000;
        }
        last_end_time = max(last_end_time, tx.end_time);
    }

    double avg_read_cycle = read_count ? (double)read_total_cycles / read_count : 0.0;
    double avg_write_cycle = write_count ? (double)write_total_cycles / write_count : 0.0;
    int total_clk_cycles = last_end_time / 10000;
    double bus_utilization = total_clk_cycles ? (double)total_transaction_cycles / total_clk_cycles * 100.0 : 0.0;

    ofstream fout(output_file);
    if (!fout.is_open())
    {
        cerr << "Failed to open output file: " << output_file << endl;
        return 3;
    }

    fout << "Number of Read Transactions with no wait states: " << read_no_wait << "\n";
    fout << "Number of Read Transactions with wait states: " << read_wait << "\n";
    fout << "Number of Write Transactions with no wait states: " << write_no_wait << "\n";
    fout << "Number of Write Transactions with wait states: " << write_wait << "\n";
    fout << "Average Read Cycle: " << fixed << setprecision(2) << avg_read_cycle << " cycles\n";
    fout << "Average Write Cycle: " << fixed << setprecision(2) << avg_write_cycle << " cycles\n";
    fout << "Bus Utilization: " << fixed << setprecision(2) << bus_utilization << "%\n";
    fout << "Number of Idle Cycles: " << idle_cycles << "\n";
    fout << "Number of Completer: " << analyzer.getCompleterCount() << "\n";
    fout << "CPU Elapsed Time: " << fixed << setprecision(2) << elapsed_ms << " ms\n\n";
    fout << "Number of Transactions with Timeout: " << timeout << "\n";
    fout << "Number of Out-of-Range Accesses: " << out_of_range_count << "\n";
    fout << "Number of Mirrored Transactions: " << mirrored_count << "\n";
    fout << "Number of Read-Write Overlap Errors: " << overlap_error_count << "\n";

    fout << "\n";
    {
        std::stringstream ss;
        analyzer.printCompleterInfo(ss);
        fout << ss.str();
    }
    fout << "\n";
    {
        std::stringstream ss;
        analyzer.generateCompleterErrorReport(ss);
        fout << ss.str();
    }
    fout.close();
    // cout << "Done. Output written to " << output_file << endl;
    return 0;
}
