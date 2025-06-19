#include "AddressCorruptionDetector.h"
#include <iostream>

void AddressCorruptionDetector::check(uint64_t timestamp, const std::unordered_map<std::string, std::string> &signals) {
    if (signals.at("psel") == "1" && signals.at("penable") == "1") {
        std::string current_addr = signals.at("paddr");

        // Compare each bit individually with last snapshot
        if (!last_seen_address_bits.empty()) {
            for (size_t i = 0; i < current_addr.size(); ++i) {
                std::string bit_name = "a" + std::to_string(i);
                std::string current_bit = std::string(1, current_addr[i]);

                if (last_seen_address_bits[bit_name] != current_bit) {
                    std::cout << "[ADDRESS CORRUPTION] Bit " << bit_name 
                              << " changed unexpectedly at timestamp " << timestamp << std::endl;

                    if (report) report->address_corruption_count++;

                    // Save this as a transaction (optional, but helps centralize reporting)
                    Transaction corruption_tx;
                    corruption_tx.start_time = timestamp;
                    corruption_tx.end_time = timestamp;
                    corruption_tx.addr = current_addr;
                    corruption_tx.type = signals.at("pwrite") == "1" ? TransactionType::WRITE : TransactionType::READ;
                    corruption_tx.has_wait_state = false;
                    corruption_tx.data = signals.at("pwdata");

                    transactions.push_back(corruption_tx);

                    break; // Report once per transaction even if multiple bits changed
                }
            }
        }

        // Update last seen address bits
        for (size_t i = 0; i < current_addr.size(); ++i) {
            std::string bit_name = "a" + std::to_string(i);
            last_seen_address_bits[bit_name] = std::string(1, current_addr[i]);
        }
    }
}
