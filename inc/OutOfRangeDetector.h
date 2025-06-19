#include "Transaction.h"
#include "HazardReport.h"
#include <vector>
#include <unordered_map>
#include <string>

class OutOfRangeDetector {
private:
    SignalHistory &signal_history;
    HazardReport *report;
    std::vector<Transaction> transactions;

    const uint32_t ADDR_MAX = 0x7F; // Example limit

public:
    OutOfRangeDetector(SignalHistory &history, HazardReport *report)
        : signal_history(history), report(report) {}

    void check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals);

    std::vector<Transaction> getTransactions() const { return transactions; }
};
