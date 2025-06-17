#ifndef TRANSACTION_ANALYZER_H
#define TRANSACTION_ANALYZER_H

#include "VCDParser.h"
#include "TransactionAnalyzer.h"
#include "HazardManager.h"
#include "SignalHistory.h"
#include "CompleterMapper.h"
#include <vector>
#include <unordered_map>
#include <string>

enum class TransactionType { READ, WRITE };

struct Transaction {
    uint64_t start_time;
    uint64_t end_time;
    std::string addr;
    std::string data;
    TransactionType type;
    bool has_wait_state;
};

class TransactionAnalyzer {
private:
    std::vector<VCDChange> changes;
    std::unordered_map<std::string, SignalInfo> symbol_table;
    std::vector<Transaction> transactions;
    SignalHistory signal_history;
    HazardManager hazard_manager;
    CompleterMapper completer_mapper; // Moved here

public:
    TransactionAnalyzer(const std::vector<VCDChange> &changes,
                        const std::unordered_map<std::string, SignalInfo> &symbols);

    void analyze();
    const std::vector<Transaction> &getTransactions() const;
    const HazardReport &getHazardReport() const;

    // Add this getter
    const std::unordered_map<int, CompleterConnectionReport> &getCompleterReports() const;
};

#endif // TRANSACTION_ANALYZER_H
