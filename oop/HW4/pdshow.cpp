#include "Diary.h"  

int main (int agrc, char* argv[]) {
    if (agrc < 2) {
        cerr << "Usage: pdshow <date>\n";
        return 1;
    }
    string date = argv[1];
    Diary diary("diary.txt");
    if (!diary.show_entry(argv[1])) {
        std::cerr << "No entry found for date: " << argv[1] << "\n";
        return 1;
    }
    return 0;
}