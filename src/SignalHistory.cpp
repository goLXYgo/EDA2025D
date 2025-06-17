#include "SignalHistory.h"
#include <iostream>

void SignalHistory::applyChange(uint64_t timestamp, const std::string &signal, const std::string &value) {
    // Update current signal value
    current_state[signal] = value;

    // Record in delta list (append to latest timestamp group if matched)
    if (!deltas.empty() && deltas.back().timestamp == timestamp) {
        deltas.back().changes[signal] = value;
    } else {
        SignalDelta delta;
        delta.timestamp = timestamp;
        delta.changes[signal] = value;
        deltas.push_back(delta);
    }
}

const std::unordered_map<std::string, std::string> &SignalHistory::getCurrentState() const {
    return current_state;
}

std::unordered_map<std::string, std::string> SignalHistory::getSnapshotAt(uint64_t timestamp) const {
    std::unordered_map<std::string, std::string> snapshot;

    // Initialize snapshot with "unknown" or default value
    for (const auto &entry : current_state) {
        snapshot[entry.first] = "X"; // Can customize to "0" or other default
    }

    // Apply deltas in order until the timestamp is reached
    for (const auto &delta : deltas) {
        if (delta.timestamp > timestamp)
            break;

        for (const auto &change : delta.changes) {
            snapshot[change.first] = change.second;
        }
    }

    return snapshot;
}

void SignalHistory::printCurrentState() const {
    std::cout << "Current Signal State:\n";
    for (const auto &entry : current_state) {
        std::cout << entry.first << " : " << entry.second << "\n";
    }
}

// SignalHistory.cpp
bool SignalHistory::alwaysMatch(const std::string &bit1, const std::string &bit2) const {
    const auto &h1 = signal_traces.at(bit1);
    const auto &h2 = signal_traces.at(bit2);

    if (h1.size() != h2.size()) return false;

    for (size_t i = 0; i < h1.size(); ++i) {
        if (h1[i].second != h2[i].second) return false;
    }
    return true;
}
