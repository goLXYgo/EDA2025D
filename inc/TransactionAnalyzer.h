#ifndef TRANSACTION_ANALYZER_H
#define TRANSACTION_ANALYZER_H

#include "VCDParser.h"
#include <vector>
#include <string>
#include <unordered_map>

enum class TransactionType
{
    READ,
    WRITE
};

struct Transaction
{
    TransactionType type;
    uint64_t start_time;
    uint64_t access_time;
    uint64_t end_time;
    std::string addr;
    std::string data;
    std::string orig_data;
    bool has_wait_state;
    bool timed_out = false;
    bool addr_corrupted = false;
    bool data_corrupted = false;
    bool out_of_range = false;
    bool data_mirrored = false;
    bool read_write_overlap = false;
};

class TransactionAnalyzer
{
public:
    TransactionAnalyzer(const std::vector<VCDChange> &vcd_changes,
                        const std::unordered_map<std::string, SignalInfo> &symtable);

    void analyze();
    const std::vector<Transaction> &getTransactions() const;

private:
    std::string getSignal(const VCDChange &change, const std::string &name);

    void checkAddressCorruption(Transaction &txn);
    void checkDataCorruption(Transaction &txn);
    void checkOutOfRange(Transaction &txn);
    void checkMirroring(Transaction &txn, std::unordered_map<std::string, std::string> &mirror_map);
    void checkOverlap(Transaction &txn);

    const std::vector<VCDChange> &changes;
    const std::unordered_map<std::string, SignalInfo> &symbol_table;

    std::unordered_map<std::string, std::string> signal_name_to_symbol;

    std::string last_psel, last_penable, last_pwrite;
    std::string last_paddr, last_pwdata, last_prdata, last_pready;

    std::vector<Transaction> transactions;
    const uint64_t timeout_threshold_ns = 100; // can be customized
};

#endif
