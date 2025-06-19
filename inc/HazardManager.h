#ifndef HAZARD_MANAGER_H
#define HAZARD_MANAGER_H

#include "SignalHistory.h"
#include "Transaction.h"
#include "HazardReport.h"

#include "TimeoutDetector.h"
#include "OutOfRangeDetector.h"
#include "DataDuplicationDetector.h"
#include "ReadWriteOverlapDetector.h"
#include "DataCorruptionDetector.h"

#include <vector>

class HazardManager {
private:
    SignalHistory &signal_history;

    TimeoutDetector timeout_detector;
    OutOfRangeDetector oor_detector;
    DataDuplicationDetector duplication_detector;
    ReadWriteOverlapDetector rw_overlap_detector;
    DataCorruptionDetector corruption_detector;

    std::vector<Transaction> transactions;
    HazardReport report;

public:
    HazardManager(SignalHistory &history);

    // Process each signal change and update all hazard records
    void processSignal(uint64_t timestamp);

    // Merge and return all parsed transactions (if multiple detectors generate them)
    std::vector<Transaction> getParsedTransactions() const;

    // Return the complete hazard report
    const HazardReport &getHazardReport() const;
};

#endif // HAZARD_MANAGER_H
