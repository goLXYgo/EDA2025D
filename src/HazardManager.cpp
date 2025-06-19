#include "HazardManager.h"

HazardManager::HazardManager(SignalHistory &history)
    : signal_history(history),
      timeout_detector(history, &report),
      oor_detector(history, &report),
      duplication_detector(history, &report),
      rw_overlap_detector(history, &report),
      corruption_detector(history, &report),
      address_corruption_detector(history, &report) {}

void HazardManager::processSignal(uint64_t timestamp) {
    const auto &current_state = signal_history.getCurrentState();

    timeout_detector.check(timestamp, current_state);
    oor_detector.check(timestamp, current_state);
    duplication_detector.check(timestamp, current_state);
    rw_overlap_detector.check(timestamp, current_state);
    corruption_detector.check(timestamp, current_state);
    address_corruption_detector.check(timestamp, current_state);
}

    // Optional: collect transactions if each detector builds them
    // Example: This can be expanded if you want to merge transactions from all detectors

std::vector<Transaction> HazardManager::getParsedTransactions() const {
    std::vector<Transaction> all;

    // Collect transactions from all detectors
    auto timeout_tx = timeout_detector.getTransactions();
    auto oor_tx = oor_detector.getTransactions();
    auto dup_tx = duplication_detector.getTransactions();
    auto overlap_tx = rw_overlap_detector.getTransactions();
    auto corruption_tx = corruption_detector.getTransactions();
	auto addr_corruption_tx = address_corruption_detector.getTransactions();

    // Merge all transactions
    all.insert(all.end(), timeout_tx.begin(), timeout_tx.end());
    all.insert(all.end(), oor_tx.begin(), oor_tx.end());
    all.insert(all.end(), dup_tx.begin(), dup_tx.end());
    all.insert(all.end(), overlap_tx.begin(), overlap_tx.end());
    all.insert(all.end(), corruption_tx.begin(), corruption_tx.end());
	all.insert(all.end(), addr_corruption_tx.begin(), addr_corruption_tx.end());

    return all;
}

std::vector<Transaction> HazardManager::getParsedTransactions() const {
    return timeout_detector.getTransactions(); // Or combine transactions from multiple detectors if needed
}

const HazardReport &HazardManager::getHazardReport() const {
    return report;
}