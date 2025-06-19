#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include "Transaction.h"
#include "HazardManager.h"
#include "CompleterMapper.h"
#include <vector>
#include <string>
#include <unordered_map>

// Writes the detailed analysis report to the specified file
void writeReport(const std::vector<Transaction> &transactions,
                 const HazardReport &hazard_report,
                 const std::unordered_map<int, CompleterConnectionReport> &completer_reports,
                 const std::string &filename);

#endif // OUTPUT_WRITER_H
