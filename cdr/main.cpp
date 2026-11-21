#include <iostream>
#include "CDRProcessor.h"

int main() {
    CDRProcessor processor;

    processor.addRecord(CallRecord(1, "03001234567", "03111234567", "VOICE", 120, 0, "2026-11-22 10:00:00"));
    processor.addRecord(CallRecord(2, "03001234567", "03111234567", "SMS", 0, 0, "2026-11-22 10:05:00"));
    processor.addRecord(CallRecord(3, "03001234567", "", "DATA", 0, 250.5, "2026-11-22 11:00:00"));
    processor.addRecord(CallRecord(3, "03001234567", "", "DATA", 0, 99, "2026-11-22 12:00:00")); // duplicate ID, rejected
    processor.addRecord(CallRecord(4, "03001234567", "03221234567", "FAX", 10, 0, "2026-11-22 12:00:00")); // invalid type, rejected

    processor.printAll();

    std::cout << "\nAccepted: " << processor.getRecordCount()
              << " | Rejected: " << processor.getRejectedCount() << "\n";
    std::cout << "Voice seconds: " << processor.getTotalVoiceSecondsFor("03001234567") << "\n";
    std::cout << "Data MB: " << processor.getTotalDataMBFor("03001234567") << "\n";
    std::cout << "SMS count: " << processor.getSmsCountFor("03001234567") << "\n";
    return 0;
}