#include "TimeoutDetector.h"
#include <iostream>

void TimeoutDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1") {
        // Start of transaction
        PendingTransaction new_tx;
        new_tx.start_time = timestamp;
        new_tx.addr = signals.at("paddr");
        pending_transactions.push_back(new_tx);
    }

    // Check if any pending transactions have timed out
    std::vector<PendingTransaction> remaining_pending;
    for (auto &tx : pending_transactions) {
        if (signals.at("pready") == "1") {
            // Transaction completed, no timeout
            Transaction completed_tx;
            completed_tx.start_time = tx.start_time;
            completed_tx.end_time = timestamp;
            completed_tx.addr = tx.addr;
            completed_tx.type = signals.at("pwrite") == "1" ? TransactionType::WRITE : TransactionType::READ;
            completed_tx.has_wait_state = (timestamp - tx.start_time) > 1;

            transactions.push_back(completed_tx);
        } else if (timestamp - tx.start_time > TIMEOUT_THRESHOLD) {
            std::cout << "[TIMEOUT] Transaction timeout at address " << tx.addr << " at time " << timestamp << std::endl;
            if (report) report->timeout_count++;
            // Timeout transaction is not added to transaction list
        } else {
            remaining_pending.push_back(tx);
        }
    }

    pending_transactions = remaining_pending;
}
