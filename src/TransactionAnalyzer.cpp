// TransactionAnalyzer.cpp
#include "../inc/TransactionAnalyzer.h"
#include "../inc/FloatingDetector.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

TransactionAnalyzer::TransactionAnalyzer() {}

bool isDataUnknown(const Transaction &txn)
{
    const std::string &data = txn.data.empty() ? txn.orig_data : txn.data;
    return data.find_first_not_of("xX") == std::string::npos;
}

std::string binToHex(const std::string &bin)
{
    std::stringstream ss;
    uint32_t val = std::stoul(bin, nullptr, 2);
    ss << "0x" << std::hex << std::uppercase << val;
    return ss.str();
}

std::unordered_set<std::string> active_completers;

void TransactionAnalyzer::feed(const VCDChange &change)
{
    for (const auto &[symbol, value] : change.changes)
    {
        signal_state[symbol] = value;
    }

    uint64_t clock_period_ps = 10000;
    uint64_t now = change.timestamp;
    string psel = getSignal("psel");
    string penable = getSignal("penable");
    string pwrite = getSignal("pwrite");
    string paddr = getSignal("paddr");
    string pwdata = getSignal("pwdata");
    string prdata = getSignal("prdata");
    string pready = getSignal("pready");

    // ✅ Debug: Show timestamp + critical signal states
    // std::cout << "[DEBUG] #" << now
    //           << " PSEL=" << psel
    //           << " PENABLE=" << penable
    //           << " PREADY=" << pready
    //           << " IN_TXN=" << (in_transaction ? "true" : "false")
    //           << std::endl;

    // 判斷是否為 Setup Phase（psel=1, penable=0）
    if (!in_transaction && psel == "1" && penable == "0")
    {
        current = Transaction{};
        current.start_time = now;
        current.addr = paddr;
        current.type = (pwrite == "1") ? TransactionType::WRITE : TransactionType::READ;
        current.orig_data = (current.type == TransactionType::WRITE) ? pwdata : prdata;
        in_transaction = true;

        std::cout << "[DEBUG] Setup Phase detected at #" << now
                  << " -> PSEL=1, PENABLE=0, Type=" << ((current.type == TransactionType::WRITE) ? "WRITE" : "READ")
                  << ", Addr=" << binToHex(paddr) << std::endl;
    }
    // 是否timeout
    if (in_transaction && (now - current.start_time >= 1000000))
    {
        current.timed_out = true;
        current.end_time = now;
        std::cout << "[DEBUG] Timeout Detected: Transaction started at #"
                  << current.start_time << ", timed out at #"
                  << now << " -> Duration = "
                  << (now - current.start_time) / clock_period_ps << " cycles, Addr = "
                  << binToHex(current.addr) << std::endl;
        in_transaction = false;
        return;
    }
    // 進入access phase
    if (in_transaction && !current.has_access_phase && psel == "1" && penable == "1")
    {
        current.has_access_phase = true;
        current.access_time = now;
        std::cout << "[DEBUG] Access Phase entered at #" << now << std::endl;
    }
    // 是否overlap
    if (in_transaction &&
        current.type == TransactionType::WRITE &&
        psel == "1" && penable == "0" && pwrite == "0")
    {
        // ⚠️ 發現 Read Transaction 想在 Write 未完成時開始 → Overlap
        Transaction fake_read;
        fake_read.type = TransactionType::READ;
        fake_read.addr = paddr;
        fake_read.start_time = now;
        fake_read.read_write_overlap = true;

        std::cout << "[ERROR] Read-Write Overlap @" << now
                  << " -> Addr = " << binToHex(fake_read.addr) << std::endl;

        // optional: 可以記到一個 overlap_errors vector 裡
        overlap_errors.push_back(fake_read);
    }
    // 完成交易
    if (in_transaction && psel == "1" && penable == "1" && pready == "1")
    {
        current.access_time = now;
        current.end_time = now;
        uint64_t delta = current.access_time - current.start_time;
        current.has_wait_state = (delta > clock_period_ps);
        current.data = (current.type == TransactionType::WRITE) ? pwdata : prdata;

        std::string addr_access = getSignal("paddr");
        std::string data_access = (current.type == TransactionType::WRITE) ? getSignal("pwdata") : getSignal("prdata");
        checkCorruptionBetweenPhases(current, addr_access, data_access);

        std::cout << "[INFO] Valid Transaction @" << current.end_time
                  << " Type=" << ((current.type == TransactionType::WRITE) ? "WRITE" : "READ")
                  << " Addr=0x" << std::hex << std::uppercase << std::stoul(current.addr, nullptr, 2)
                  << " Data=0x" << std::hex << std::uppercase << std::stoul(current.data, nullptr, 2)
                  << " Wait=" << std::dec << current.has_wait_state << std::endl;

        finalize(current);
        in_transaction = false;
    }
}
void TransactionAnalyzer::finalize(Transaction &txn)
{
    txn.completer_id = guessCompleterID(txn.addr);
    txn.expected_addr = getExpectedAddress(txn.addr);
    // [Ensure] 初始化 completer detectors，即使沒有 corruption
    if (completer_addr_detectors.count(txn.completer_id) == 0)
        completer_addr_detectors[txn.completer_id] = FloatingBitPairDetector();
    if (completer_data_detectors.count(txn.completer_id) == 0)
        completer_data_detectors[txn.completer_id] = FloatingBitPairDetector();

    if (txn.type == TransactionType::WRITE)
    {
        write_history_map[txn.addr] = txn.data;
        written_addrs.insert(txn.addr);
    }

    checkOutOfRange(txn);

    checkMirroring(txn); // ✅ 改為合併版本

    transactions.push_back(txn);
}

void TransactionAnalyzer::checkCorruptionBetweenPhases(Transaction &txn,
                                                       const std::string &addr_access,
                                                       const std::string &data_access)
{
    if (txn.type == TransactionType::READ)
        return; // READ 不做 corruption 檢查
    if (txn.orig_data.empty() || data_access.empty())
    {
        // 無法判斷 corruption，視為無誤
        return;
    }
    if (!addr_access.empty() && txn.addr != addr_access)
    {
        completer_addr_detectors[txn.completer_id].addExample(txn.addr, addr_access);
        txn.addr_corrupted = true;
        std::cout << "[ADDR_CORRUPTION] Completer " << txn.completer_id
                  << " @ #" << txn.end_time
                  << " -> Setup: " << txn.addr << " | Access: " << addr_access << std::endl;
    }

    if (!data_access.empty() && txn.orig_data != data_access)
    {
        completer_data_detectors[txn.completer_id].addExample(txn.orig_data, data_access);
        txn.data_corrupted = true;
        std::cout << "[DATA_CORRUPTION] Completer " << txn.completer_id
                  << " @ #" << txn.end_time
                  << " -> Setup: " << txn.orig_data << " | Access: " << data_access << std::endl;
    }
}

void TransactionAnalyzer::checkOutOfRange(Transaction &txn)
{
    try
    {
        // Step 0: 補齊二進位字串為 32 bits
        std::string padded_addr = txn.addr;
        if (padded_addr.length() < 32)
            padded_addr = std::string(32 - padded_addr.length(), '0') + padded_addr;

        // Step 1: 將 binary string 轉為 uint32_t
        uint32_t addr = std::stoul(padded_addr, nullptr, 2);

        // Step 2: 判斷 Completer Slot (依照 upper bits)
        std::string completer;
        bool in_range = false;

        if ((addr >= 0x1A100000) && (addr <= 0x1A100FFF))
        {
            completer = "UART";
            in_range = true;
        }
        else if ((addr >= 0x1A101000) && (addr <= 0x1A101FFF))
        {
            completer = "GPIO";
            in_range = true;
        }
        else if ((addr >= 0x1A102000) && (addr <= 0x1A102FFF))
        {
            completer = "SPI";
            in_range = true;
        }
        else
        {
            completer = "Unknown";
            in_range = false;
        }

        if (in_range) // 在三個COMPLETER範圍內，但DATA=X，代表所在的completer沒有連接
        {
            if (isDataUnknown(txn))
            {
                txn.completer_unconnected = true;
                std::cout << "[#" << txn.end_time << "] Unknown Response -> PADDR 0x"
                          << std::hex << addr << " Completer '" << completer << "' appears unconnected in this trace" << std::endl;
            }
            else
            {
                active_completers.insert(completer);
            }
        }
        // Step 3: 若不合法，記錄為 Out-of-Range
        else
        {
            txn.out_of_range = true;
            std::cout << "[#" << txn.end_time << "] Out-of-Range Access -> PADDR 0x"
                      << std::hex << std::uppercase << addr
                      << " (Requester 1 -> " << completer << ")" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Failed to parse/check PADDR: " << txn.addr
                  << " -> " << e.what() << std::endl;
    }
}

void TransactionAnalyzer::setSymbolTable(const std::unordered_map<std::string, SignalInfo> &table)
{
    for (const auto &[symbol, info] : table)
    {
        name_to_symbol[info.name] = symbol;
    }
}
void TransactionAnalyzer::checkMirroring(Transaction &txn) // done
{
    if (txn.type == TransactionType::WRITE)
    {
        // 這裡不做任何鏡像錯誤判定，只記錄
        mirror_map[txn.addr] = txn.data;

        // 記錄該筆資料的合法來源
        if (mirrored_write_values.find(txn.data) == mirrored_write_values.end())
        {
            mirrored_write_values[txn.data] = {txn.addr, txn.end_time};
        }
    }

    if (txn.type == TransactionType::READ)
    {
        // 該地址未被合法寫入過
        if (written_addrs.count(txn.addr) == 0)
        {
            auto it = mirrored_write_values.find(txn.data);
            if (it != mirrored_write_values.end())
            {
                const auto &[src_addr, src_time] = it->second;

                std::cout << "[#" << txn.end_time << "] Data Mirroring -> Value "
                          << binToHex(txn.data) << " written at PADDR "
                          << binToHex(src_addr) << " also found at PADDR "
                          << binToHex(txn.addr) << std::endl;

                txn.data_mirrored = true;
                txn.mirrored_with_addr = binToHex(src_addr);
            }
        }
    }
}

// void TransactionAnalyzer::checkOverlap(Transaction &txn)
// {
//     if (txn.type != TransactionType::READ)
//         return;

//     if (transactions.empty())
//         return;

//     const Transaction &prev = transactions.back();

//     // 檢查上一筆是否為寫入，且對同一個地址，且尚未在 txn 發生前完成
//     if (prev.type == TransactionType::WRITE &&
//         prev.addr == txn.addr &&
//         prev.end_time >= txn.start_time)
//     {
//         txn.read_write_overlap = true;

//         std::cout << "[ERROR] Read-Write Overlap @" << txn.start_time
//                   << " → Addr = " << txn.addr << std::endl;
//     }
// }
std::string TransactionAnalyzer::getSignal(const std::string &name) const
{
    auto it = name_to_symbol.find(name);
    if (it == name_to_symbol.end())
        return "";
    auto sit = signal_state.find(it->second);
    return (sit != signal_state.end()) ? sit->second : "";
}

int TransactionAnalyzer::guessCompleterID(const std::string &addr_bin)
{
    uint32_t addr = std::stoul(addr_bin, nullptr, 2);
    if (addr >= 0x1A100000 && addr <= 0x1A100FFF)
        return 1;
    if (addr >= 0x1A101000 && addr <= 0x1A101FFF)
        return 2;
    if (addr >= 0x1A102000 && addr <= 0x1A102FFF)
        return 3;
    return 0;
}

std::string TransactionAnalyzer::getExpectedAddress(const std::string &actual_bin)
{
    return actual_bin; // TODO: replace with ground truth logic
}

void TransactionAnalyzer::reportAddressCorruptions()
{
    for (const auto &[cid, detector] : completer_addr_detectors)
    {
        std::cout << "Completer " << cid << " Address Floating: ";
        if (detector.found())
            std::cout << "a" << detector.bit1() << "-a" << detector.bit2() << std::endl;
        else
            std::cout << "Not detected" << std::endl;
    }
}

void TransactionAnalyzer::reportDataCorruptions()
{
    for (const auto &[cid, detector] : completer_data_detectors)
    {
        std::cout << "Completer " << cid << " Data Floating: ";
        if (detector.found())
            std::cout << "d" << detector.bit1() << "-d" << detector.bit2() << std::endl;
        else
            std::cout << "Not detected" << std::endl;
    }
}

const std::vector<Transaction> &TransactionAnalyzer::getTransactions() const
{
    return transactions;
}

void TransactionAnalyzer::reportActiveCompleters()
{
    std::cout << "Number of Completers: " << active_completers.size() << std::endl;
    for (const auto &c : active_completers)
    {
        std::cout << "-> Completer Detected: " << c << std::endl;
    }
}

int TransactionAnalyzer::getCompleterCount() const
{
    return active_completers.size();
}
void TransactionAnalyzer::printCompleterInfo(std::ostream &os)
{
    std::cout << "[DEBUG] Generating Completer connection report..." << std::endl;
    std::cout << "[DEBUG] #Completer Addr Detectors = " << completer_addr_detectors.size() << std::endl;
    std::cout << "[DEBUG] #Completer Data Detectors = " << completer_data_detectors.size() << std::endl;

    std::unordered_set<int> all_completer_ids;
    for (const auto &[cid, _] : completer_addr_detectors)
        all_completer_ids.insert(cid);
    for (const auto &[cid, _] : completer_data_detectors)
        all_completer_ids.insert(cid);
    for (int cid : all_completer_ids)
    {
        os << "Completer " << cid << " PADDR Connections\n";
        auto it_addr = completer_addr_detectors.find(cid);
        int a1 = -1, a2 = -1;
        if (it_addr != completer_addr_detectors.end() && it_addr->second.found())
        {
            a1 = it_addr->second.bit1();
            a2 = it_addr->second.bit2();
        }
        for (int i = 7; i >= 0; --i)
        {
            if (i == a1)
                os << "a" << i << ": Connected with a" << a2 << "\n";
            else if (i == a2)
                os << "a" << i << ": Connected with a" << a1 << "\n";
            else
                os << "a" << i << ": Correct\n";
        }
        os << "\n";

        os << "Completer " << cid << " PWDATA Connections\n";
        auto it_data = completer_data_detectors.find(cid);
        int d1 = -1, d2 = -1;
        if (it_data != completer_data_detectors.end() && it_data->second.found())
        {
            d1 = it_data->second.bit1();
            d2 = it_data->second.bit2();
        }
        for (int i = 7; i >= 0; --i)
        {
            if (i == d1)
                os << "d" << i << ": Connected with d" << d2 << "\n";
            else if (i == d2)
                os << "d" << i << ": Connected with d" << d1 << "\n";
            else
                os << "d" << i << ": Correct\n";
        }
        os << "\n";
    }
}
