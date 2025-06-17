// =========================
// apb_vcd_analyzer/TransactionAnalyzer.cpp
// =========================
#include "../inc/TransactionAnalyzer.h"
#include <iostream>

TransactionAnalyzer::TransactionAnalyzer(const std::vector<VCDChange> &changes,
                                         const std::unordered_map<std::string, SignalInfo> &symbol_table)
    : changes(changes), symbol_table(symbol_table), hazard_manager(signal_history) {}
/*
TransactionAnalyzer::TransactionAnalyzer(const vector<VCDChange> &changes, const unordered_map<string, SignalInfo> &symbol_table)
    : changes(changes), symbol_table(symbol_table) {}
*/
void TransactionAnalyzer::analyze() {
    for (const auto &change_set : changes) {
        uint64_t timestamp = change_set.timestamp;

        for (const auto &change : change_set.changes) {
            signal_history.applyChange(timestamp, change.first, change.second);
        }

        // Pass the current signal state to the hazard manager
        hazard_manager.processSignal(timestamp);
    }

    transactions = hazard_manager.getParsedTransactions();

    // Add this line to analyze completer connections
    completer_mapper.analyze();
}
/*
void TransactionAnalyzer::analyze() {
    extractTransactions();
}
*/

void TransactionAnalyzer::extractTransactions() {
    for (const auto &change : changes) {
        // Dummy extraction logic, needs proper signal tracking.
        Transaction tx;
        tx.start_time = change.timestamp;
        tx.addr = "0x00";
        tx.data = "0x00";
        tx.type = TransactionType::WRITE;
        tx.has_wait_state = false;
        transactions.push_back(tx);
    }
}


const std::vector<Transaction> &TransactionAnalyzer::getTransactions() const {
    return transactions;
}

const HazardReport &TransactionAnalyzer::getHazardReport() const {
    return hazard_manager.getHazardReport();
}

const std::unordered_map<int, CompleterConnectionReport> &TransactionAnalyzer::getCompleterReports() const {
    return completer_mapper.getReports();
}

/*
const vector<Transaction> &TransactionAnalyzer::getTransactions() const {
    return transactions;
}
*/

