#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>

using namespace std;

// 定义房间类型的枚举
enum RoomType {
    NORMAL = 0,
    START = 1,
    PRINCESS = 2,
    MONSTER = 3,
    EXIT = 4
};

// 房间类
class Room {
public:
    Room* north;
    Room* south;
    Room* east;
    Room* west;
    int x, y;
    RoomType roomType;
    string name;

    Room(int x, int y, const string& name)
        : north(nullptr), south(nullptr), east(nullptr), west(nullptr), x(x), y(y), roomType(NORMAL), name(name) {}

    // 获取房间的出口
    string getExits();
    // 打印房间信息
    void printRoomInfo();
};

// 地图类
class Map {
private:
    int size;
    vector<vector<Room*>> rooms;
    Room* startRoom;
    Room* princessRoom;
    Room* monsterRoom;
    Room* exitRoom;   // 唯一的出口房间
    Room* currentRoom;
    mt19937 rng;

public:
    Map(int n) : size(n), rooms(n, vector<Room*>(n, nullptr)), rng(random_device{}()) {
        initializeRooms();
        generateSpecialRooms();
        currentRoom = startRoom;  // 玩家从起始房间开始
    }

    // 初始化房间并生成房间名称
    void initializeRooms();
    // 随机选择特殊房间（起始房间、公主房间、怪物房间和唯一出口房间）
    void generateSpecialRooms();
    // 玩家移动函数
    void movePlayer(const string& direction);

    // 检查当前房间类型并显示相关信息
    void checkRoom();

    // 启动游戏
    void startGame();

    // 析构函数，清理内存
    ~Map() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                delete rooms[i][j];
            }
        }
    }
};


