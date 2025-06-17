// =========================
// apb_vcd_analyzer/inc/Transaction.h
// =========================
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <cstdint>

enum class TransactionType { READ, WRITE };

struct Transaction {
    uint64_t start_time = 0;   // Timestamp when the transaction started
    uint64_t end_time = 0;     // Timestamp when the transaction ended
    std::string addr = "0x00"; // Address involved in the transaction
    std::string data = "0x00"; // Data involved in the transaction
    TransactionType type = TransactionType::WRITE; // Transaction type: READ or WRITE
    bool has_wait_state = false; // Indicates if the transaction had wait states
};

#endif // TRANSACTION_H
