#ifndef CDRPROCESSOR_H
#define CDRPROCESSOR_H

#include <vector>
#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "CallRecord.h"

// Usage of one subscriber in one month
struct MonthlyUsage {
    std::string subscriber;
    std::string month;        // "YYYY-MM"
    int voiceSeconds;
    int smsCount;
    double dataMB;

    MonthlyUsage() : voiceSeconds(0), smsCount(0), dataMB(0.0) {}
};

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

    // Groups all records by subscriber and month (sorted by subscriber, then month)
    std::vector<MonthlyUsage> getMonthlySummary() const {
        std::map<std::pair<std::string, std::string>, MonthlyUsage> grouped;

        for (const CallRecord& r : records) {
            std::string month = r.getTimestamp().substr(0, 7);   // "YYYY-MM"
            std::pair<std::string, std::string> key(r.getCallerNumber(), month);

            MonthlyUsage& u = grouped[key];
            u.subscriber = r.getCallerNumber();
            u.month = month;

            if (r.getCallType() == "VOICE")     u.voiceSeconds += r.getDurationSeconds();
            else if (r.getCallType() == "SMS")  u.smsCount++;
            else if (r.getCallType() == "DATA") u.dataMB += r.getDataUsedMB();
        }

        std::vector<MonthlyUsage> result;
        for (const auto& entry : grouped) result.push_back(entry.second);
        return result;
    }

    void printMonthlySummary() const {
        std::vector<MonthlyUsage> summary = getMonthlySummary();

        std::cout << std::left << std::setw(14) << "Subscriber"
                  << std::setw(10) << "Month"
                  << std::setw(14) << "Voice (min)"
                  << std::setw(6) << "SMS"
                  << "Data (MB)\n";

        for (const MonthlyUsage& u : summary) {
            std::cout << std::left << std::setw(14) << u.subscriber
                      << std::setw(10) << u.month
                      << std::setw(14) << (u.voiceSeconds / 60.0)
                      << std::setw(6) << u.smsCount
                      << u.dataMB << "\n";
        }
    }

    void printAll() const {
        for (const CallRecord& r : records) r.display();
    }
};

#endif