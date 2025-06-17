// ============================
// apb_vcd_analyzer/inc/SignalSnapshot.h
// ============================

#ifndef SIGNAL_SNAPSHOT_H
#define SIGNAL_SNAPSHOT_H

#include <unordered_map>
#include <string>
#include <cstdint>

struct SignalSnapshot
{
    uint64_t timestamp; // The timestamp when this snapshot was taken
    std::unordered_map<std::string, std::string> signal_values; // Signal name → value mapping
};

#endif // SIGNAL_SNAPSHOT_H
