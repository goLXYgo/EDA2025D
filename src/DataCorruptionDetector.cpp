#include "DataCorruptionDetector.h"
#include <iostream>

void DataCorruptionDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1") {
        std::string addr = signals.at("paddr");
        std::string pwrite = signals.at("pwrite");

        if (pwrite == "1") {
            // Write phase: record expected data
            std::string pwdata = signals.at("pwdata");
            last_written_data[addr] = pwdata;
        } else {
            // Read phase: verify against last written data
            std::string prdata = signals.at("prdata");

            if (last_written_data.find(addr) != last_written_data.end() && last_written_data[addr] != prdata) {
                std::cout << "[DATA CORRUPTION] Address " << addr
                          << " expected: " << last_written_data[addr]
                          << " read: " << prdata
                          << " at timestamp " << timestamp << std::endl;

                if (report) report->corruption_count++;

                // Log detailed hazard transaction
                Transaction tx;
                tx.start_time = timestamp;
                tx.end_time = timestamp;
                tx.addr = addr;
                tx.data = "Expected: " + last_written_data[addr] + ", Read: " + prdata;
                tx.type = TransactionType::READ;
                tx.has_wait_state = false;

                transactions.push_back(tx);
            }
        }
    }
}
