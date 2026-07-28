#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <numeric>

using namespace std;

// 定义学生结构体
struct Student {
    string name;
    vector<double> scores;
    double average;
};

// 读取成绩记录文件并返回学生列表
vector<Student> readRecords(const string& filename) {
    vector<Student> students;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return students;
    }

    while (getline(file, line)) {
        istringstream iss(line);
        string name;
        double score;
        Student student;

        iss >> name;
        student.name = name;

        while (iss >> score) {
            student.scores.push_back(score);
        }

        // 计算平均分
        double sum = accumulate(student.scores.begin(), student.scores.end(), 0.0);
        student.average = sum / student.scores.size();

        students.push_back(student);
    }

    file.close();
    return students;
}

// 打印学生成绩总结
void printSummary(const vector<Student>& students) {
    cout << left << setw(10) << "no"
              << setw(10) << "name"
              << setw(10) << "score1"
              << setw(10) << "score2"
              << setw(10) << "score3"
              << setw(10) << "average" << endl;

    for (size_t i = 0; i < students.size(); ++i) {
        const auto& student = students[i];
        cout << left << setw(10) << (i + 1)
                  << setw(10) << student.name;
        for (double score : student.scores) {
            cout << setw(10) << score;
        }
        cout << setw(10) << fixed << setprecision(2) << student.average << endl;
    }

    // 计算总平均分、最小值和最大值
    vector<double> totalAverages(students[0].scores.size(), 0.0);
    vector<double> minScores(students[0].scores.size(), 100.0);
    vector<double> maxScores(students[0].scores.size(), 0.0);

    for (const auto& student : students) {
        for (size_t j = 0; j < student.scores.size(); ++j) {
            totalAverages[j] += student.scores[j];
            if (student.scores[j] < minScores[j]) minScores[j] = student.scores[j];
            if (student.scores[j] > maxScores[j]) maxScores[j] = student.scores[j];
        }
    }

    for (double& avg : totalAverages) {
        avg /= students.size();
    }

    cout << left << setw(10) << "average";
    for (double avg : totalAverages) {
        cout << setw(10) << fixed << setprecision(2) << avg;
    }
    cout << endl;

    cout << left << setw(10) << "min";
    for (double min : minScores) {
        cout << setw(10) << min;
    }
    cout << endl;

    cout << left << setw(10) << "max";
    for (double max : maxScores) {
        cout << setw(10) << max;
    }
    cout << endl;
}

int main() {
    string filename = "input.txt";  // 成绩记录文件名
    vector<Student> students = readRecords(filename);
    printSummary(students);
    return 0;
}