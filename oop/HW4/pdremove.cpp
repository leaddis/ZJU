#include "Diary.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: pdremove <date>\n";
        return -1;
    }
    Diary diary("diary.txt");
    if(!diary.delete_entry(argv[1])) {
        std::cerr << "No entry found for date: " << argv[1] << "\n";
        return -1;
    }
    return 0;
};