#ifndef ADDRESS_CORRUPTION_DETECTOR_H
#define ADDRESS_CORRUPTION_DETECTOR_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <string>

class AddressCorruptionDetector {
private:
    SignalHistory &signal_history;
    std::vector<Transaction> transactions;
    HazardReport *report;

    std::unordered_map<std::string, std::string> last_seen_address_bits;

public:
    AddressCorruptionDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};

#endif // ADDRESS_CORRUPTION_DETECTOR_H
