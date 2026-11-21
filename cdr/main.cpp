#include <iostream>
#include <string>
#include "CDRProcessor.h"

int main() {
    CDRProcessor processor;

    int loaded = processor.loadFromCSV("sample_cdr.csv");
    if (loaded < 0) {
        std::cout << "Could not open sample_cdr.csv. Run this program from inside the cdr folder.\n";
        return 1;
    }

    processor.printAll();
    std::cout << "\nLoaded: " << loaded
              << " | Rejected: " << processor.getRejectedCount() << "\n\n";

    const std::string numbers[2] = {"03001234567", "03331234567"};
    for (const std::string& n : numbers) {
        std::cout << n
                  << " -> Voice: " << processor.getTotalVoiceSecondsFor(n) << "s"
                  << " | SMS: " << processor.getSmsCountFor(n)
                  << " | Data: " << processor.getTotalDataMBFor(n) << " MB\n";
    }
    return 0;
}