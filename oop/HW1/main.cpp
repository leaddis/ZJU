#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// 定义一个结构体来保存学生记录
struct Student {
    string name;
    int score1, score2, score3;
    float average;
};

// 计算三门课程的平均分
float calculateAverage(int s1, int s2, int s3) {
    return (s1 + s2 + s3) / 3.0f;
}

int main() {
    vector<Student> students;
    int sum_score1 = 0, sum_score2 = 0, sum_score3 = 0;
    
    // 将最小值初始化为题目中的最高分 5，最大值初始化为题目中的最低分 1
    int min_score1 = 5, max_score1 = 1;
    int min_score2 = 5, max_score2 = 1;
    int min_score3 = 5, max_score3 = 1;

    // 输入10个学生的记录
    for (int i = 0; i < 10; ++i) {
        Student student;
        cout << "请输入学生 " << i + 1 << " 的姓名和3门课程的成绩 (格式: 姓名 成绩1 成绩2 成绩3): ";
        cin >> student.name >> student.score1 >> student.score2 >> student.score3;

        // 计算该学生的平均分
        student.average = calculateAverage(student.score1, student.score2, student.score3);

        // 添加学生记录到列表
        students.push_back(student);

        // 计算每门课程的总分
        sum_score1 += student.score1;
        sum_score2 += student.score2;
        sum_score3 += student.score3;

        // 更新课程1的最小值和最大值
        if (student.score1 < min_score1) min_score1 = student.score1;
        if (student.score1 > max_score1) max_score1 = student.score1;

        // 更新课程2的最小值和最大值
        if (student.score2 < min_score2) min_score2 = student.score2;
        if (student.score2 > max_score2) max_score2 = student.score2;

        // 更新课程3的最小值和最大值
        if (student.score3 < min_score3) min_score3 = student.score3;
        if (student.score3 > max_score3) max_score3 = student.score3;
    }

    // 输出学生记录及其平均分
    cout << setw(5) << "no" << setw(10) << "name" << setw(10) << "score1" 
         << setw(10) << "score2" << setw(10) << "score3" << setw(10) << "average" << endl;

    for (size_t i = 0; i < students.size(); ++i) {
        cout << setw(5) << i + 1 << setw(10) << students[i].name 
             << setw(10) << students[i].score1 << setw(10) << students[i].score2 
             << setw(10) << students[i].score3 << setw(10) << fixed << setprecision(5) 
             << students[i].average << endl;
    }

    // 计算并输出每门课程的平均分、最小值和最大值
    cout << setw(15) << "average" << setw(10) << fixed << setprecision(1) 
         << (sum_score1 / 10.0) << setw(10) << (sum_score2 / 10.0) 
         << setw(10) << (sum_score3 / 10.0) << endl;

    cout << setw(15) << "min" << setw(10) << min_score1 << setw(10) << min_score2 
         << setw(10) << min_score3 << endl;

    cout << setw(15) << "max" << setw(10) << max_score1 << setw(10) << max_score2 
         << setw(10) << max_score3 << endl;

    return 0;
}
