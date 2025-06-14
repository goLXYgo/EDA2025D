#ifndef REPORT_WRITER_H
#define REPORT_WRITER_H

#include "TransactionAnalyzer.h"
#include <string>
#include <vector>

void writeReport(const std::vector<Transaction> &transactions, const std::string &output_file);

#endif // REPORT_WRITER_H
