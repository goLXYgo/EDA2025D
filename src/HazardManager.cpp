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

    // You can collect transactions as needed here.
    // For now, if transactions are built by a specific detector, retrieve them from it.
}

std::vector<Transaction> HazardManager::getParsedTransactions() const {
    return timeout_detector.getTransactions(); // Example: or combine results from all detectors
}

const HazardReport &HazardManager::getHazardReport() const {
    return report;
}
