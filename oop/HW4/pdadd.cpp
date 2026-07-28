#include "Diary.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: pdadd <date>\n";
        return 1;
    }

    string date = argv[1];
    Diary diary("diary.txt");

    string line, content;
    while (getline(cin, line)) {
        if (line == ".") 
            break;
        content += line + "\n";
    }

    diary.add_entry(date, content);
    return 0;
}