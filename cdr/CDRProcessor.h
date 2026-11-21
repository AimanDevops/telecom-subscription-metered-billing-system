#ifndef CDRPROCESSOR_H
#define CDRPROCESSOR_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "CallRecord.h"

class CDRProcessor {
private:
    std::vector<CallRecord> records;
    int rejectedCount;

    bool idExists(int id) const {
        for (const CallRecord& r : records) {
            if (r.getRecordId() == id) return true;
        }
        return false;
    }

public:
    CDRProcessor() : rejectedCount(0) {}

    // Returns true if the record was accepted
    bool addRecord(const CallRecord& record) {
        if (!record.isValid() || idExists(record.getRecordId())) {
            rejectedCount++;
            return false;
        }
        records.push_back(record);
        return true;
    }

    // Loads records from a CSV file.
    // Returns the number of accepted records, or -1 if the file cannot be opened.
    // CSV columns: id,caller,receiver,type,duration,dataMB,timestamp
    int loadFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return -1;

        std::string line;
        int loaded = 0;
        bool isHeader = true;

        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (isHeader) { isHeader = false; continue; }
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string f[7];
            bool ok = true;
            for (int i = 0; i < 7; i++) {
                if (!std::getline(ss, f[i], ',')) { ok = false; break; }
            }
            if (!ok) { rejectedCount++; continue; }

            try {
                int id = std::stoi(f[0]);
                int duration = std::stoi(f[4]);
                double dataMB = std::stod(f[5]);
                if (addRecord(CallRecord(id, f[1], f[2], f[3], duration, dataMB, f[6])))
                    loaded++;
            } catch (...) {
                rejectedCount++;   // non-numeric id, duration or data value
            }
        }
        return loaded;
    }

    int getRecordCount() const { return (int)records.size(); }
    int getRejectedCount() const { return rejectedCount; }

    std::vector<CallRecord> getRecordsFor(const std::string& number) const {
        std::vector<CallRecord> result;
        for (const CallRecord& r : records) {
            if (r.getCallerNumber() == number) result.push_back(r);
        }
        return result;
    }

    int getTotalVoiceSecondsFor(const std::string& number) const {
        int total = 0;
        for (const CallRecord& r : records) {
            if (r.getCallerNumber() == number && r.getCallType() == "VOICE")
                total += r.getDurationSeconds();
        }
        return total;
    }

    double getTotalDataMBFor(const std::string& number) const {
        double total = 0.0;
        for (const CallRecord& r : records) {
            if (r.getCallerNumber() == number && r.getCallType() == "DATA")
                total += r.getDataUsedMB();
        }
        return total;
    }

    int getSmsCountFor(const std::string& number) const {
        int count = 0;
        for (const CallRecord& r : records) {
            if (r.getCallerNumber() == number && r.getCallType() == "SMS") count++;
        }
        return count;
    }

    void printAll() const {
        for (const CallRecord& r : records) r.display();
    }
};

#endif