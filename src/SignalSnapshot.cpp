// ============================
// apb_vcd_analyzer/src/SignalSnapshot.cpp
// ============================
#include "../inc/SignalSnapshot.h"
#include <iostream>

void printSnapshot(const SignalSnapshot &snapshot) {
    std::cout << "Timestamp: " << snapshot.timestamp << std::endl;
    for (const auto &pair : snapshot.signal_values) {
        std::cout << "  " << pair.first << " = " << pair.second << std::endl;
    }
}

bool compareSnapshots(const SignalSnapshot &a, const SignalSnapshot &b) {
    if (a.signal_values.size() != b.signal_values.size()) return false;

    for (const auto &pair : a.signal_values) {
        auto it = b.signal_values.find(pair.first);
        if (it == b.signal_values.end() || it->second != pair.second) {
            return false;
        }
    }
    return true;
}
