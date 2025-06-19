#ifndef READ_WRITE_OVERLAP_DETECTOR_H
#define READ_WRITE_OVERLAP_DETECTOR_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

class ReadWriteOverlapDetector {
private:
    SignalHistory &signal_history;
    HazardReport *report;

    std::unordered_map<std::string, bool> active_writes;

    std::vector<Transaction> transactions;

public:
    ReadWriteOverlapDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};

#endif // READ_WRITE_OVERLAP_DETECTOR_H
