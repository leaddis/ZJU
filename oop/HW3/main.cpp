#include "map.h"

int main() {
    int size;
    cout << "enter your map size:";
    cin >> size;
    Map map(size);  // 创建一个size x size的地图
    map.startGame();  // 开始游戏
    return 0;
}