#include "map.h"

string Room::getExits() {
    string exits;
    if (north) exits += "north ";
    if (south) exits += "south ";
    if (east) exits += "east ";
    if (west) exits += "west ";
    return exits;
};

void Room::printRoomInfo() {
    cout << "Welcome to " << name << ". ";
    if (roomType == EXIT) {
        cout << "This is the exit! You can leave the castle from here!\n";
    } else {
        string exits = getExits();
        // 手动统计空格的数量（表示出口的数量）
        int exitCount = 0;
        for (char ch : exits) {
            if (ch == ' ') exitCount++;
        }
        cout << "There are " << exitCount << " exits: " << exits << "\n";
    }
}

void Map::initializeRooms() {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            string roomName = "Room" + to_string(i * size + j);
            rooms[i][j] = new Room(i, j, roomName);
        }
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i > 0) rooms[i][j]->north = rooms[i - 1][j];
            if (i < size - 1) rooms[i][j]->south = rooms[i + 1][j];
            if (j > 0) rooms[i][j]->west = rooms[i][j - 1];
            if (j < size - 1) rooms[i][j]->east = rooms[i][j + 1];
        }
    }
}

void Map::generateSpecialRooms() {
    uniform_int_distribution<int> dist(0, size - 1);

    // 随机生成开始房间
    int startX = dist(rng);
    int startY = dist(rng);
    startRoom = rooms[startX][startY];
    startRoom->roomType = START;
    startRoom->name = "lobby";

    // 随机选择公主房间
    do {
        int princessX = dist(rng);
        int princessY = dist(rng);
        princessRoom = rooms[princessX][princessY];
    } while (princessRoom == startRoom);
    princessRoom->roomType = PRINCESS;
    princessRoom->name = "Secret Room";

    // 随机选择怪物房间
    do {
        int monsterX = dist(rng);
        int monsterY = dist(rng);
        monsterRoom = rooms[monsterX][monsterY];
    } while (monsterRoom == startRoom || monsterRoom == princessRoom);
    monsterRoom->roomType = MONSTER;
    monsterRoom->name = "Dungeon";

    // 随机选择唯一出口房间，并且出口房间必须在边界上
    do {
        int exitX, exitY;
        // 随机选择边界房间（i = 0, j = 0, i = size-1, j = size-1）
        uniform_int_distribution<int> edgeDist(0, 3);
        int edge = edgeDist(rng);

        if (edge == 0) {
            exitX = 0; // 上边界
            exitY = dist(rng);
        } else if (edge == 1) {
            exitX = size - 1; // 下边界
            exitY = dist(rng);
        } else if (edge == 2) {
            exitX = dist(rng);
            exitY = 0; // 左边界
        } else {
            exitX = dist(rng);
            exitY = size - 1; // 右边界
        }

        exitRoom = rooms[exitX][exitY];
    } while (exitRoom == startRoom || exitRoom == princessRoom || exitRoom == monsterRoom);
    exitRoom->roomType = EXIT;
    exitRoom->name = "Exit Room";
}

void Map::movePlayer(const string& direction) {
    Room* nextRoom = nullptr;
    if (direction == "north" && currentRoom->north) nextRoom = currentRoom->north;
    if (direction == "south" && currentRoom->south) nextRoom = currentRoom->south;
    if (direction == "east" && currentRoom->east) nextRoom = currentRoom->east;
    if (direction == "west" && currentRoom->west) nextRoom = currentRoom->west;

    if (nextRoom) {
        currentRoom = nextRoom;
        checkRoom();
    } else {
        cout << "You can't go that way!\n";
    }
}

void Map::checkRoom() {
    currentRoom->printRoomInfo();

    if (currentRoom->roomType == MONSTER) {
        cout << "Oh no! You encountered a monster! Game over.\n";
        exit(0);
    }

    if (currentRoom->roomType == PRINCESS) {
        cout << "You found the princess! She says: 'Please take me back to the Hall!'\n";
    }

    if (currentRoom->roomType == EXIT) {
        cout << "Congratulations! You found the exit and successfully escaped the castle!\n";
        exit(0);
    }
}

void Map::startGame() {
    currentRoom->printRoomInfo();
    string command, direction;

    while (true) {
        cout << "Enter your command: ";
        cin >> command >> direction;

        if (command == "go") {
            movePlayer(direction);
        } else {
            cout << "Unknown command. Please use 'go' followed by a direction (north, south, east, west).\n";
        }
    }
}

