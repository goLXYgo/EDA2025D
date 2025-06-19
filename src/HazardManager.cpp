#include "HazardManager.h"

HazardManager::HazardManager(SignalHistory &history)
    : signal_history(history),
      timeout_detector(history),
      oor_detector(history),
      duplication_detector(history),
      rw_overlap_detector(history),
      corruption_detector(history) {}

void HazardManager::processSignal(uint64_t timestamp) {
    const auto &current_state = signal_history.getCurrentState();

    timeout_detector.check(timestamp, current_state);
    oor_detector.check(timestamp, current_state);
    duplication_detector.check(timestamp, current_state);
    rw_overlap_detector.check(timestamp, current_state);
    corruption_detector.check(timestamp, current_state);

    // Optional: collect transactions if each detector builds them
    // Example: This can be expanded if you want to merge transactions from all detectors
}

std::vector<Transaction> HazardManager::getParsedTransactions() const {
    // Merge transactions from all detectors (if applicable)
    std::vector<Transaction> all;

    auto t1 = timeout_detector.getTransactions();
    auto t2 = oor_detector.getTransactions();
    auto t3 = duplication_detector.getTransactions();
    auto t4 = rw_overlap_detector.getTransactions();
    auto t5 = corruption_detector.getTransactions();

    all.insert(all.end(), t1.begin(), t1.end());
    all.insert(all.end(), t2.begin(), t2.end());
    all.insert(all.end(), t3.begin(), t3.end());
    all.insert(all.end(), t4.begin(), t4.end());
    all.insert(all.end(), t5.begin(), t5.end());

    return all;
}

const HazardReport &HazardManager::getHazardReport() const {
    return report;
}
