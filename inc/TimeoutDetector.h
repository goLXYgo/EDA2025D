#ifndef TIMEOUT_DETECTOR_H
#define TIMEOUT_DETECTOR_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

class TimeoutDetector {
private:
    SignalHistory &signal_history;
    std::vector<Transaction> transactions;
    HazardReport *report;

    struct PendingTransaction {
        uint64_t start_time;
        std::string addr;
        bool waiting = true;
    };

    std::vector<PendingTransaction> pending_transactions;

    const uint64_t TIMEOUT_THRESHOLD = 100; // 100 cycles timeout

public:
    TimeoutDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};

#endif // TIMEOUT_DETECTOR_H
