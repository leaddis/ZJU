#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

class Diary {
    private:
        string file_path;
        map<string, string> diary;
        void loadDiary();
        void saveDiary();
    public: 
        Diary(const string file_path);
        bool add_entry(const string date, const string content);
        bool delete_entry(const string date);
        bool show_entry(const string date) const;
        void list_entries(const string start_date = " ", const string end_date = " ") const;

};
