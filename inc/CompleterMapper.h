#ifndef COMPLETER_MAPPER_H
#define COMPLETER_MAPPER_H

#include "SignalHistory.h"
#include "SignalSnapshot.h" // (optional but might be useful in implementation)

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

// Represents the connection status of an individual bit
struct BitConnectionStatus {
    std::string status; // "Correct" or "Connected with aX/dX"
};

// Holds connection status reports for all address and data bits for a completer
struct CompleterConnectionReport {
    std::unordered_map<std::string, BitConnectionStatus> paddr_lines;
    std::unordered_map<std::string, BitConnectionStatus> pwdata_lines;
};

// Handles the analysis of PADDR and PWDATA connection consistency for each completer
class CompleterMapper {
private:
    SignalHistory &signal_history;
    std::unordered_map<int, CompleterConnectionReport> completer_reports;

public:
    CompleterMapper(SignalHistory &history);

    void analyze();
    const std::unordered_map<int, CompleterConnectionReport> &getReports() const;

private:
    void analyzePADDRLines(int completer_id);
    void analyzePWDATALines(int completer_id);
};

#endif // COMPLETER_MAPPER_H
