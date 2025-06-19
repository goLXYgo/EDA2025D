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
#include "AddressCorruptionDetector.h"

#include <vector>

class HazardManager {
private:
    SignalHistory &signal_history;
    HazardReport report; // Single shared report

    TimeoutDetector timeout_detector;
    OutOfRangeDetector oor_detector;
    DataDuplicationDetector duplication_detector;
    ReadWriteOverlapDetector rw_overlap_detector;
    DataCorruptionDetector corruption_detector;
	AddressCorruptionDetector address_corruption_detector;

    std::vector<Transaction> transactions;

public:
    HazardManager(SignalHistory &history);

    void processSignal(uint64_t timestamp);

    std::vector<Transaction> getParsedTransactions() const;
    const HazardReport &getHazardReport() const;
};

#endif // HAZARD_MANAGER_H
