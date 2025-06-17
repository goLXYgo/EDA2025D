#ifndef COMPLETER_MAPPER_H
#define COMPLETER_MAPPER_H

#include "SignalHistory.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

struct BitConnectionStatus {
    std::string status; // "Correct" or "Connected with aX/dX"
};

struct CompleterConnectionReport {
    std::unordered_map<std::string, BitConnectionStatus> paddr_lines;
    std::unordered_map<std::string, BitConnectionStatus> pwdata_lines;
};

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
