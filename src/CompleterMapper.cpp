#include "CompleterMapper.h"
#include <iostream>
#include <unordered_set>

CompleterMapper::CompleterMapper(SignalHistory &history)
    : signal_history(history) {}

void CompleterMapper::analyze() {
    // Example: assuming Completer 1 and Completer 2 for now.
    analyzePADDRLines(1);
    analyzePWDATALines(1);
    analyzePADDRLines(2);
    analyzePWDATALines(2);
}

void CompleterMapper::analyzePADDRLines(int completer_id) {
    auto &current_state = signal_history.getCurrentState();
    CompleterConnectionReport &report = completer_reports[completer_id];

    for (int i = 0; i < 8; ++i) {
        std::string bit_name = "a" + std::to_string(i);
        report.paddr_lines[bit_name].status = "Correct"; // Default

        // Simple floating detection example: compare if two address bits always change together
        for (int j = i + 1; j < 8; ++j) {
            std::string other_bit = "a" + std::to_string(j);
            if (signal_history.alwaysMatch(bit_name, other_bit)) {
                report.paddr_lines[bit_name].status = "Connected with " + other_bit;
                report.paddr_lines[other_bit].status = "Connected with " + bit_name;
            }
        }
    }
}

void CompleterMapper::analyzePWDATALines(int completer_id) {
    auto &current_state = signal_history.getCurrentState();
    CompleterConnectionReport &report = completer_reports[completer_id];

    for (int i = 0; i < 8; ++i) {
        std::string bit_name = "d" + std::to_string(i);
        report.pwdata_lines[bit_name].status = "Correct";

        for (int j = i + 1; j < 8; ++j) {
            std::string other_bit = "d" + std::to_string(j);
            if (signal_history.alwaysMatch(bit_name, other_bit)) {
                report.pwdata_lines[bit_name].status = "Connected with " + other_bit;
                report.pwdata_lines[other_bit].status = "Connected with " + bit_name;
            }
        }
    }
}

const std::unordered_map<int, CompleterConnectionReport> &CompleterMapper::getReports() const {
    return completer_reports;
}
