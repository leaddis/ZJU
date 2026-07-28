#include "Diary.h"

Diary::Diary(const string file_path):file_path(file_path) {
    loadDiary();
}

// Load diary from file
void Diary::loadDiary(){// (#2021-03-01,......)
    ifstream file(file_path);
    string date, content,line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') {
            if (!date.empty()) diary[date] = content;
            date = line.substr(1);
            content.clear();
        } else {
            content += line + "\n";
        }
    }
    if (!date.empty()) diary[date] = content;
    file.close();
}

// Save diary to file
void Diary::saveDiary() {
    std::ofstream file(file_path);
    for (const auto& [date, content] : diary) {
        file << "#" << date << "\n" << content << "\n";
    }
    file.close();
}

// Add entry to diary
bool Diary::add_entry(const string date, const string content) {
    //if (diary.find(date) != diary.end()) return false;
    diary[date] = content;
    saveDiary();
    return true;
}

// Delete entry from diary
bool Diary::delete_entry(const string date) {
    if (diary.erase(date) > 0) {
        saveDiary();
        return true;
    }
    return false;
}

// Show entry from diary
bool Diary::show_entry(const string date) const {
    auto it = diary.find(date);
    if (it != diary.end()) {
        std::cout << it->second;
        return true;
    }
    return false;
}

// List entries from diary
void Diary::list_entries(const string start_date, const string end_date) const {
    for (const auto& [date, content] : diary) {
        if ((start_date.empty() || date >= start_date) && (end_date.empty() || date <= end_date)) {
            cout << date << "\n";
        }
    }
}