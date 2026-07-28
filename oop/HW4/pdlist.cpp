#include "Diary.h"

int main(int argc, char* argv[]) { 
    Diary diary("diary.txt");
    if (argc == 3){
        diary.list_entries(argv[1], argv[2]);
    }
    else{
        diary.list_entries();
    }
    return 0;
}