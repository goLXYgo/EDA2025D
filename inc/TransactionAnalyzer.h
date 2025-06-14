#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include "Common.h"
#include "FloatingDetector.h"

enum class TransactionType
{
    READ,
    WRITE
};

struct Transaction
{
    uint64_t start_time = 0;
    uint64_t access_time = 0;
    uint64_t end_time = 0;
    bool has_access_phase = false;
    bool has_wait_state = false;
    bool timed_out = false;
    bool addr_corrupted = false;
    bool data_corrupted = false;
    bool out_of_range = false;
    bool data_mirrored = false;
    bool read_write_overlap = false;
    bool addr_mirrored = false;     // 標記 Address Mirroring，對應 [#W] 的時間點
    std::string mirrored_with_addr; // 鏡像對象的地址（ex: 0x36 or 0x32）
    TransactionType type;
    std::string addr;
    std::string expected_addr;
    std::string orig_data;
    std::string data;
    int completer_id = -1;
    bool completer_unconnected;
};

class TransactionAnalyzer
{
public:
    TransactionAnalyzer();
    void feed(const VCDChange &change);
    const std::vector<Transaction> &getTransactions() const;
    void reportActiveCompleters();
    void reportAddressCorruptions();
    void reportDataCorruptions();
    int getCompleterCount() const;
    void setSymbolTable(const std::unordered_map<std::string, SignalInfo> &table);
    void printCompleterInfo(std::ostream &os);

private:
    void finalize(Transaction &txn);
    void checkOutOfRange(Transaction &txn);
    void checkMirroring(Transaction &txn);
    // void checkOverlap(Transaction &txn);
    void checkCorruptionBetweenPhases(Transaction &txn,
                                      const std::string &addr_access,
                                      const std::string &data_access);

    // 用來記錄某個值首次寫在哪個位址、哪個時間點，for mirroring
    std::unordered_map<std::string, std::pair<std::string, uint64_t>> mirrored_write_values;
    // 格式：value → {addr, time}
    std::vector<Transaction> overlap_errors;
    std::string getSignal(const std::string &name) const;
    int guessCompleterID(const std::string &addr_bin);
    std::string getExpectedAddress(const std::string &actual_bin);
    std::unordered_map<std::string, std::tuple<std::string, std::string, uint64_t>> mirror_pending_reads;
    std::unordered_set<std::string> written_addrs;
    std::unordered_map<std::string, std::string> write_history_map; // 合法的寫入紀錄
    std::unordered_map<std::string, std::string> signal_state;
    std::unordered_map<std::string, std::string> mirror_map;
    std::unordered_map<std::string, std::string> name_to_symbol = {
        {"clk", "#"},
        {"rst_n", "$"},
        {"paddr", "%"},
        {"pwdata", "&"},
        {"pwrite", "'"},
        {"psel", "("},
        {"penable", ")"},
        {"pready", "*"},
        {"prdata", "+"}};

    std::unordered_map<int, FloatingBitPairDetector> completer_addr_detectors;
    std::unordered_map<int, FloatingBitPairDetector> completer_data_detectors;

    std::vector<Transaction> transactions;
    Transaction current;
    bool in_transaction = false;
    const uint64_t timeout_threshold_ps = 1000000;
};