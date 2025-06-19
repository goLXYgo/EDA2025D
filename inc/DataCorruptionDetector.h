#ifndef DATA_CORRUPTION_DETECTOR_H
#define DATA_CORRUPTION_DETECTOR_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

class DataCorruptionDetector {
private:
    SignalHistory &signal_history;
    HazardReport *report;

    std::unordered_map<std::string, std::string> last_written_data;

    std::vector<Transaction> transactions;

public:
    DataCorruptionDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};

#endif // DATA_CORRUPTION_DETECTOR_H
