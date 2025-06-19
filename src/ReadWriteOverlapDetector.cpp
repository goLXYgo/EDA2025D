#include "ReadWriteOverlapDetector.h"
#include <iostream>

void ReadWriteOverlapDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1") {
        std::string addr = signals.at("paddr");
        std::string pwrite = signals.at("pwrite");

        if (pwrite == "1") {
            // Write detected
            active_writes[addr] = true;
        } else {
            // Read detected
            if (active_writes.find(addr) != active_writes.end() && active_writes[addr]) {
                std::cout << "[READ-WRITE OVERLAP] Read and Write overlap at address " << addr
                          << " at timestamp " << timestamp << std::endl;

                if (report) report->overlap_count++;

                // Log detailed hazard transaction
                Transaction tx;
                tx.start_time = timestamp;
                tx.end_time = timestamp;
                tx.addr = addr;
                tx.data = "Read-Write Overlap Detected";
                tx.type = TransactionType::READ;
                tx.has_wait_state = false;

                transactions.push_back(tx);
            }
        }
    }

    // Clean up finished writes (assuming transaction ends when psel and penable are deasserted)
    if (signals.at("psel") == "0" && signals.at("penable") == "0") {
        active_writes.clear();
    }
}
