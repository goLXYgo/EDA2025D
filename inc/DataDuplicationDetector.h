#ifndef DATA_DUPLICATION_DETECTOR_H
#define DATA_DUPLICATION_DETECTOR_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

class DataDuplicationDetector {
private:
    SignalHistory &signal_history;
    HazardReport *report;

    std::unordered_map<std::string, std::vector<std::pair<uint64_t, std::string>>> recent_writes;

    std::vector<Transaction> transactions;

public:
    DataDuplicationDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};

#endif // DATA_DUPLICATION_DETECTOR_H
