#ifndef SIGNAL_SNAPSHOT_H
#define SIGNAL_SNAPSHOT_H

#include <unordered_map>
#include <string>
#include <cstdint>

struct SignalSnapshot {
    uint64_t timestamp; // The timestamp when this snapshot was taken
    std::unordered_map<std::string, std::string> signal_values; // Signal name → value mapping

    // Add this constructor
    SignalSnapshot(uint64_t ts, const std::unordered_map<std::string, std::string> &signals)
        : timestamp(ts), signal_values(signals) {}

    // Optional: keep the default constructor for flexibility
    SignalSnapshot() = default;
};

#endif // SIGNAL_SNAPSHOT_H
