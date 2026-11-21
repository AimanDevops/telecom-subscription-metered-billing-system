#ifndef CALLRECORD_H
#define CALLRECORD_H

#include <iostream>
#include <string>

class CallRecord {
private:
    int recordId;
    std::string callerNumber;
    std::string receiverNumber;
    std::string callType;      // "VOICE", "SMS" or "DATA"
    int durationSeconds;       // used for VOICE
    double dataUsedMB;         // used for DATA
    std::string timestamp;     // "YYYY-MM-DD HH:MM:SS"

public:
    CallRecord() : recordId(0), durationSeconds(0), dataUsedMB(0.0) {}

    CallRecord(int id, const std::string& caller, const std::string& receiver,
               const std::string& type, int duration, double dataMB,
               const std::string& time)
        : recordId(id), callerNumber(caller), receiverNumber(receiver),
          callType(type), durationSeconds(duration), dataUsedMB(dataMB),
          timestamp(time) {}

    // Getters
    int getRecordId() const { return recordId; }
    std::string getCallerNumber() const { return callerNumber; }
    std::string getReceiverNumber() const { return receiverNumber; }
    std::string getCallType() const { return callType; }
    int getDurationSeconds() const { return durationSeconds; }
    double getDataUsedMB() const { return dataUsedMB; }
    std::string getTimestamp() const { return timestamp; }

    // Setters with basic protection
    void setDurationSeconds(int d) { if (d >= 0) durationSeconds = d; }
    void setDataUsedMB(double mb) { if (mb >= 0) dataUsedMB = mb; }

    bool isValid() const {
        bool typeOk = (callType == "VOICE" || callType == "SMS" || callType == "DATA");
        return recordId > 0 && !callerNumber.empty() && typeOk &&
               durationSeconds >= 0 && dataUsedMB >= 0 && !timestamp.empty();
    }

    void display() const {
        std::cout << "[" << recordId << "] " << timestamp << " | "
                  << callerNumber << " -> " << receiverNumber << " | "
                  << callType << " | " << durationSeconds << "s | "
                  << dataUsedMB << " MB\n";
    }
};

#endif