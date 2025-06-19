#include "DataDuplicationDetector.h"
#include <iostream>

void DataDuplicationDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1" && signals.at("pwrite") == "1") {
        std::string addr = signals.at("paddr");
        std::string data = signals.at("pwdata");

        if (recent_writes.find(addr) != recent_writes.end()) {
            for (const auto &entry : recent_writes[addr]) {
                if (entry.second == data) {
                    std::cout << "[DATA DUPLICATION] Address " << addr
                              << " written with duplicated data " << data
                              << " at timestamp " << timestamp << std::endl;

                    if (report) report->mirrored_count++;

                    // Log detailed hazard transaction
                    Transaction tx;
                    tx.start_time = timestamp;
                    tx.end_time = timestamp;
                    tx.addr = addr;
                    tx.data = data;
                    tx.type = TransactionType::WRITE;
                    tx.has_wait_state = false;

                    transactions.push_back(tx);
                    break;
                }
            }
        }

        // Store this write for future duplication checks
        recent_writes[addr].emplace_back(timestamp, data);
    }
}
