#ifndef CDRPROCESSOR_H
#define CDRPROCESSOR_H

#include <vector>
#include <string>
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