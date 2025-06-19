#ifndef SIGNAL_HISTORY_H
#define SIGNAL_HISTORY_H

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

struct SignalDelta {
    uint64_t timestamp;
    std::unordered_map<std::string, std::string> changes;
};

class SignalHistory {
private:
    std::unordered_map<std::string, std::string> current_state; // live signal values
    std::vector<SignalDelta> deltas; // history of changes

    // ✅ Add signal trace history for alwaysMatch
    std::unordered_map<std::string, std::vector<std::pair<uint64_t, std::string>>> signal_traces;

public:
    // Apply a signal change at a specific timestamp
    void applyChange(uint64_t timestamp, const std::string &signal, const std::string &value);

    // Get the latest snapshot (the current live signals)
    const std::unordered_map<std::string, std::string> &getCurrentState() const;

    // Reconstruct the signal state at a given timestamp
    std::unordered_map<std::string, std::string> getSnapshotAt(uint64_t timestamp) const;

    // Provide a SignalSnapshot object at a given timestamp
    SignalSnapshot getSnapshot(uint64_t timestamp) const {
        return SignalSnapshot(timestamp, getSnapshotAt(timestamp));
    }

    // For debugging: dump the signal state
    void printCurrentState() const;

    bool alwaysMatch(const std::string &bit1, const std::string &bit2) const;
};

#endif // SIGNAL_HISTORY_H
