// ============================
// apb_vcd_analyzer/inc/HazardReport.h
// ============================

#ifndef HAZARD_REPORT_H
#define HAZARD_REPORT_H

#include <vector>
#include <string>

struct HazardEntry {
    uint64_t timestamp;
    std::string description;
};

struct HazardReport {
    int timeout_count = 0;
    int out_of_range_count = 0;
    int mirrored_count = 0; // For address or data mirroring
    int overlap_count = 0;
    int data_corruption_count = 0;

    std::vector<HazardEntry> hazard_logs; // For detailed log entries
};

#endif // HAZARD_REPORT_H
