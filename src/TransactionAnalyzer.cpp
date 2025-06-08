#include "../inc/TransactionAnalyzer.h"
#include <iostream>
#include <unordered_set>

using namespace std;

TransactionAnalyzer::TransactionAnalyzer(const vector<VCDChange> &vcd_changes,
                                         const unordered_map<string, SignalInfo> &symtable)
    : changes(vcd_changes), symbol_table(symtable)
{
    for (const auto &[symbol, info] : symbol_table)
    {
        signal_name_to_symbol[info.name] = symbol;
    }
}

string TransactionAnalyzer::getSignal(const VCDChange &change, const string &name)
{
    auto it = signal_name_to_symbol.find(name);
    if (it == signal_name_to_symbol.end())
        return "";

    for (const auto &[symbol, value] : change.changes)
    {
        if (symbol == it->second)
            return value;
    }
    return "";
}

void TransactionAnalyzer::analyze()
{
    bool in_transaction = false;
    Transaction current;
    unordered_map<string, string> active_data; // address -> data for mirroring check

    for (const auto &change : changes)
    {
        string psel = getSignal(change, "psel");
        string penable = getSignal(change, "penable");
        string pwrite = getSignal(change, "pwrite");
        string paddr = getSignal(change, "paddr");
        string pwdata = getSignal(change, "pwdata");
        string prdata = getSignal(change, "prdata");
        string pready = getSignal(change, "pready");

        if (!psel.empty())
            last_psel = psel;
        if (!penable.empty())
            last_penable = penable;
        if (!pwrite.empty())
            last_pwrite = pwrite;
        if (!paddr.empty())
            last_paddr = paddr;
        if (!pwdata.empty())
            last_pwdata = pwdata;
        if (!prdata.empty())
            last_prdata = prdata;
        if (!pready.empty())
            last_pready = pready;

        if (!in_transaction && last_psel == "1" && last_penable == "0")
        {
            in_transaction = true;
            current = Transaction{};
            current.start_time = change.timestamp;
            current.type = (last_pwrite == "1") ? TransactionType::WRITE : TransactionType::READ;
            current.addr = last_paddr;
            current.orig_data = (current.type == TransactionType::WRITE) ? last_pwdata : last_prdata;
        }

        // Timeout check
        if (in_transaction && change.timestamp - current.start_time > timeout_threshold_ns)
        {
            current.timed_out = true;
            current.end_time = change.timestamp;
            transactions.push_back(current);
            cout << "[TIMEOUT] @" << current.start_time << " addr=" << current.addr << endl;
            in_transaction = false;
            continue;
        }

        if (in_transaction && last_psel == "1" && last_penable == "1" && last_pready == "1")
        {
            current.access_time = change.timestamp;
            current.end_time = change.timestamp;
            current.has_wait_state = (current.access_time - current.start_time > 1);
            current.data = (current.type == TransactionType::WRITE) ? last_pwdata : last_prdata;

            checkAddressCorruption(current);
            checkDataCorruption(current);
            checkOutOfRange(current);
            checkMirroring(current, active_data);
            checkOverlap(current);

            transactions.push_back(current);
            in_transaction = false;
        }
    }

    if (in_transaction)
    {
        cout << "[WARN] Incomplete transaction starting at t=" << current.start_time << endl;
    }
}

void TransactionAnalyzer::checkAddressCorruption(Transaction &txn)
{
    if (txn.addr.size() >= 6 && txn.addr[txn.addr.size() - 5] != txn.addr[txn.addr.size() - 4])
    {
        txn.addr_corrupted = true;
        cout << "[ERROR] Address Corruption @" << txn.end_time << " → Expected=" << txn.addr << " with a5-a4 floating." << endl;
    }
}

void TransactionAnalyzer::checkDataCorruption(Transaction &txn)
{
    if (txn.type == TransactionType::WRITE && txn.data.size() >= 4 && txn.data[txn.data.size() - 4] != txn.data[txn.data.size() - 3])
    {
        txn.data_corrupted = true;
        cout << "[ERROR] Data Corruption @" << txn.end_time << " → Expected=" << txn.orig_data << " got=" << txn.data << " with d3-d2 floating." << endl;
    }
}

void TransactionAnalyzer::checkOutOfRange(Transaction &txn)
{
    int addr_val = stoi(txn.addr, nullptr, 2);
    if (addr_val > 127)
    {
        txn.out_of_range = true;
        cout << "[ERROR] Out-of-Range @" << txn.end_time << " → Addr=" << addr_val << endl;
    }
}

void TransactionAnalyzer::checkMirroring(Transaction &txn, unordered_map<string, string> &mirror_map)
{
    for (const auto &[addr, data] : mirror_map)
    {
        if (data == txn.data && addr != txn.addr)
        {
            txn.data_mirrored = true;
            cout << "[ERROR] Data Mirroring @" << txn.end_time << " → Value=" << txn.data << " also at Addr=" << addr << endl;
        }
    }
    if (txn.type == TransactionType::WRITE)
    {
        mirror_map[txn.addr] = txn.data;
    }
}

void TransactionAnalyzer::checkOverlap(Transaction &txn)
{
    for (const auto &prev : transactions)
    {
        if (prev.end_time >= txn.start_time && prev.addr == txn.addr)
        {
            txn.read_write_overlap = true;
            cout << "[ERROR] Read-Write Overlap @" << txn.start_time << " with Addr=" << txn.addr << endl;
        }
    }
}

const vector<Transaction> &TransactionAnalyzer::getTransactions() const
{
    return transactions;
}
