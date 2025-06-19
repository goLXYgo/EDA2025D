#include "OutOfRangeDetector.h"
#include <iostream>

void OutOfRangeDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1") {
        std::string paddr_bin = signals.at("paddr");
        uint32_t paddr_val = std::stoul(paddr_bin, nullptr, 2);

        if (paddr_val > ADDR_MAX) {
            std::cout << "[OUT-OF-RANGE] Address 0x" << std::hex << paddr_val
                      << " accessed at timestamp " << timestamp << std::endl;

            if (report) report->out_of_range_count++;

            // Build transaction record
            Transaction tx;
            tx.start_time = timestamp;
            tx.end_time = timestamp;
            tx.addr = "0x" + paddr_bin; // or format as hex if needed
            tx.data = "UNKNOWN";
            tx.type = signals.at("pwrite") == "1" ? TransactionType::WRITE : TransactionType::READ;
            tx.has_wait_state = false;

            transactions.push_back(tx);
        }
    }
}
