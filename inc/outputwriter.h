#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include "TransactionAnalyzer.h"
#include "HazardManager.h"
#include "CompleterMapper.h"
#include <vector>
#include <string>
#include <unordered_map>

void writeReport(const std::vector<Transaction> &transactions,
                 const HazardReport &hazard_report,
                 const std::unordered_map<int, CompleterConnectionReport> &completer_reports,
                 const std::string &filename);

#endif // OUTPUT_WRITER_H
