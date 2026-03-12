// game_pract_sp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <random>

#define ID_PLAYER 1
#define ID_ENEMY 2
#define ID_CHEST 3
#define ID_TRAP 4
#define ID_DOOR 5
#define ID_WALL 6
#define RED "\033[31m"
#define WHITE "\033[0m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CHANCE_FOR_DOORS 50


struct Player {
    int x;
    int y;
    int x_room;
    int y_room;
    int hp = 1000;
};

struct Enemy{
    int id;
    int x;
    int y;
    int health;
    int damage;
};

struct Trap{
    int id;
    int x;
    int y;
    int damage;
};

struct Chest{
    int id;
    int x;
    int y;
};

union Elements {
    Enemy enemy;
    Trap trap;
    Chest chest;
};

struct Room {
public:
    int id = 0;

    int x;
    int y;

    bool solved = false;//пройдена ли комната

    bool up = false, down = false, left = false, right = false;//стороны где могут появиться проходы

    Player* player;

    int place[10][10] = {};//размеры комнаты

    Elements elements[5];//элементы которые могут появиться в комнате
};

Room* map[10][10] = {nullptr};

void GenerateEnemy(Elements *element) {
    element->enemy.damage = 50;
    element->enemy.health = 500;
}

void GenerateTrap(Elements *element) {
    element->trap.damage = 30;
}

void GenerateChest() {

}

//макет генерации
void GenerateRoom(Room* room, Player* player, int x, int y) {


    room->player = player;

    room->id = 1;

    map[room->x][room->y] = room;

    //присваивание двери через которубю вошли
    if (y == 0)
        room->down = true;
    if (y == 9)
        room->up = true;
    if (x == 0)
        room->right = true;
    if (x == 9)
        room->left = true;

    std::random_device rd;   // non-deterministic generator
    std::mt19937 gen(rd());

    if (gen() % 100 <= CHANCE_FOR_DOORS) {
        room->right = true;
    }
    if (gen() % 100 <= CHANCE_FOR_DOORS) {
        room->left = true;
    }
    if (gen() % 100 <= CHANCE_FOR_DOORS) {
        room->up = true;
    }
    if (gen() % 100 <= CHANCE_FOR_DOORS) {
        room->down = true;
    }

    if (room->x == 0)
        room->left = false;
    else if (room->x == 9)
        room->right = false;
    if (room->y == 0)
        room->up = false;
    else if (room->y == 9)
        room->down = false;

    bool canLeft = true, canRight = true, canUp = true, canDown = true;
    int countAnotherRooms = 0;
    if (map[room->x - 1][room->y] != nullptr) {
        if (map[room->x - 1][room->y]->right == true)
            room->left = true;
        else {
            room->left = false;
            canLeft = false;
            countAnotherRooms++;
        }
    }
    if (map[room->x + 1][room->y] != nullptr) {
        if (map[room->x + 1][room->y]->left == true)
            room->right = true;
        else {
            room->right = false;
            canRight = false;
            countAnotherRooms++;
        }
    }
    if (map[room->x][room->y - 1] != nullptr) {
        if (map[room->x][room->y - 1]->down == true)
            room->up = true;
        else {
            room->up = false;
            canUp = false;
            countAnotherRooms++;
        }
    }
    if (map[room->x][room->y + 1] != nullptr) {
        if (map[room->x][room->y + 1]->up == true)
            room->down = true;
        else {
            room->down = false;
            canDown = false;
            countAnotherRooms++;
        }
    }

    int countDoors = 0;
    if (room->left) countDoors++;
    if (room->right) countDoors++;
    if (room->up) countDoors++;
    if (room->down) countDoors++;

    if (countDoors <= 1 && countAnotherRooms <= 2) {
        int rnd = gen() % 75;
        while (true) {
            if (rnd <= 25) {
                if (room->left) {
                    if (room->x == 9 || canRight == false) rnd = 50;
                    else room->right = true;
                }
                else if (room->right) {
                    if (room->x == 0 || canLeft == false) rnd = 50;
                    else room->left = true;
                }
                else if (room->down) {
                    if (room->y == 0 || canUp == false) rnd = 50;
                    else room->up = true;
                }
                else if (room->up) {
                    if (room->y == 9 || canDown == false) rnd = 50;
                    else room->down = true;
                }
            }
            if (rnd > 25 && rnd <= 50) {
                if (room->left) {
                    if (room->y == 9 || canDown == false) rnd = 75;
                    else room->down = true;
                }
                else if (room->right) {
                    if (room->y == 0 || canUp == false) rnd = 75;
                    else room->up = true;
                }
                else if (room->down) {
                    if (room->x == 9 || canRight == false) rnd = 75;
                    else room->right = true;
                }
                else if (room->up) {
                    if (room->x == 0 || canLeft == false) rnd = 75;
                    else room->left = true;
                }
            }
            if (rnd > 50) {
                if (room->left) {
                    if (room->y == 0 || canUp == false) {
                        rnd = 25;
                        continue;
                    }
                    else room->up = true;
                }
                else if (room->right) {
                    if (room->y == 9 || canDown == false) {
                        rnd = 25;
                        continue;
                    }
                    else room->down = true;
                }
                else if (room->down) {
                    if (room->x == 0 || canLeft == false) {
                        rnd = 25;
                        continue;
                    }
                    else room->left = true;
                }
                else if (room->up) {
                    if (room->x == 9 || canRight == false) {
                        rnd = 25;
                        continue;
                    }
                    else room->right = true;
                }
            }
            break;
        }
    }


    //функция вызова шансов 
    Enemy enemy;
    enemy.x = 1;
    enemy.y = 2;
    enemy.id = ID_ENEMY;

    Chest chest;
    chest.x = 6;
    chest.y = 2;
    chest.id = ID_CHEST;

    Trap trap;
    trap.x = 5;
    trap.y = 4;
    trap.id = ID_TRAP;

    room->elements[0].enemy = enemy;
    GenerateEnemy(&room->elements[0]);
    room->elements[1].chest = chest;
    room->elements[2].trap = trap;
    GenerateTrap(&room->elements[2]);
}

void PrintPlace(Room* room) {

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            room->place[i][j] = 0;
        }
    }


    //установка элементов

    bool onTrap = false;

    if (room->elements[0].enemy.health >0)
        room->place[room->elements[0].enemy.x][room->elements[0].enemy.y] = ID_ENEMY;
    
    room->place[room->elements[1].chest.x][room->elements[1].chest.y] = ID_CHEST;

    room->place[room->player->x][room->player->y] = ID_PLAYER;

    for (int i = 2; i < 5; i++) {
        if (room->elements[i].trap.id == ID_TRAP) {
            if (room->place[room->elements[i].trap.x][room->elements[i].trap.y] == 0)
                room->place[room->elements[i].trap.x][room->elements[i].trap.y] = ID_TRAP;
            else if (room->place[room->elements[i].trap.x][room->elements[i].trap.y] == ID_PLAYER)
                onTrap = true;
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i == 0 || i == 9 || j == 0 || j == 9) {
                room->place[j][i] = ID_WALL;
            }
        }
    }

    if (room->up) {
        room->place[4][0] = ID_DOOR;
        room->place[5][0] = ID_DOOR;
    }
    if (room->down) {
        room->place[4][9] = ID_DOOR;
        room->place[5][9] = ID_DOOR;
    }
    if (room->left) {
        room->place[0][4] = ID_DOOR;
        room->place[0][5] = ID_DOOR;
    }
    if (room->right) {
        room->place[9][4] = ID_DOOR;
        room->place[9][5] = ID_DOOR;
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (room->place[j][i] == ID_CHEST)
                std::cout << (char)127 << " ";
            else if (room->place[j][i] == ID_WALL)
                std::cout << (char)254 << " ";
            else if (room->place[j][i] == ID_DOOR) {
                if (room->solved)
                    std::cout << GREEN << (char)254 << WHITE << " ";
                else
                    std::cout << RED << (char)254 << WHITE << " ";
            }
            else if (room->place[j][i] == ID_ENEMY)
                std::cout << RED << (char)253 << WHITE << " ";
            else if (room->place[j][i] == ID_PLAYER) {
                if (onTrap)
                    std::cout << YELLOW << (char)253 << WHITE <<" ";
                else
                    std::cout << (char)253 <<" ";
            }
            else
                std::cout << (char)255 << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    //for (int i = 0; i < 10; i++) {
    //    for (int j = 0; j < 10; j++) {
    //        std::cout << room->place[j][i] << " ";
    //    }
    //    std::cout << std::endl;
    //}

    std::cout << "hp player - " << room->player->hp << std::endl;
    std::cout << "enemy player - " << room->elements[0].enemy.health << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (map[j][i] != nullptr) {
                std::cout << map[j][i]->id << " ";
            }
            else std::cout << 0 << " ";
        }
        std::cout << std::endl;
    }

}


DWORD WINAPI EnemyGameplay(LPVOID _room) {
    
    Room* room = (Room*)_room;
    Enemy* enemy = &room->elements[0].enemy;
    Player* player = room->player;

    while (enemy->health >0) {

        if (player->x < enemy->x) {
            if (player->x != enemy->x - 1 || player->y != enemy->y) 
                enemy->x--;
        }
        else if (player->x > enemy->x) {
            if (player->x != enemy->x + 1 || player->y != enemy->y)
                enemy->x++;
        }
        if (player->y < enemy->y) {
            if (player->x != enemy->x || player->y != enemy->y - 1)
                enemy->y--;
        }
        else if (player->y > enemy->y) {
            if (player->x != enemy->x || player->y != enemy->y + 1)
                enemy->y++;
        }

        Sleep(700);
    }
    
    room->solved = true;

    return 0;
}

DWORD WINAPI EnemyThread(LPVOID _room) {
    HANDLE hEvent;
    hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"FightEvent");
    if (hEvent == NULL)
        return GetLastError();

    WaitForSingleObject(hEvent, INFINITE);

    Room* room = (Room*)_room;

    room->elements[0].enemy.health = 0;
}

int main()
{
    HANDLE hEvent[4];
    hEvent[0] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"FightEvent");
    hEvent[1] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"ChestEvent");
    hEvent[2] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"TrapEvent");
    hEvent[3] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"DoorEvent");
    if (hEvent[0] == NULL || hEvent[1] == NULL || hEvent[2] == NULL || hEvent[3] == NULL)
        return GetLastError();

    Room startRoom;
    Player player;

    startRoom.player = &player;

    startRoom.player->x = 4;
    startRoom.player->y = 8;

    startRoom.x = 4;
    startRoom.y = 4;

    map[startRoom.x][startRoom.y] = &startRoom;

    GenerateRoom(&startRoom, startRoom.player ,4,9);
    startRoom.left = true;
    startRoom.right = true;
    startRoom.down = true;
    startRoom.up = true;

    /*for (int i = 0; i < 256; i++) {
        char s = i;
        std::cout << s << " - " << i << std::endl;
    }*/
    Room* playingRoom = &startRoom;
    int x = 0, y = 0;
    while (true) {

        if (playingRoom->id != 1)
            GenerateRoom(playingRoom, &player, x, y);

        HANDLE hThreads[2];
        DWORD IDThreads[2];

        hThreads[0] = CreateThread(NULL, 0, EnemyGameplay, playingRoom, NULL, &IDThreads[0]);
        if (hThreads[0] == NULL)
            return GetLastError();

        hThreads[1] = CreateThread(NULL, 0, EnemyThread, playingRoom, NULL, &IDThreads[1]);
        if (hThreads[1] == NULL)
            return GetLastError();

        bool solvedRoom = false;
        while (solvedRoom != true) {
            PrintPlace(playingRoom);

            if (_kbhit() == 1) {
                switch (_getch()) {
                case 'w':
                    switch (playingRoom->place[playingRoom->player->x][playingRoom->player->y - 1]) {
                    case 0:
                        playingRoom->player->y--;
                        break;
                    case ID_ENEMY:
                        PulseEvent(hEvent[0]);
                        break;
                    case ID_CHEST:
                        PulseEvent(hEvent[1]);
                        break;
                    case ID_DOOR:
                        if (playingRoom->solved == true) {
                            solvedRoom = true;
                            Room *rm;
                            if (map[playingRoom->x][playingRoom->y - 1] != nullptr) {
                                rm = map[playingRoom->x][playingRoom->y - 1];
                            }
                            else {
                                rm = new Room;
                            }
                            x = playingRoom->player->x;
                            y = playingRoom->player->y - 1;
                            rm->player = playingRoom->player;
                            rm->player->y = playingRoom->player->y + 7;
                            rm->x = playingRoom->x;
                            rm->y = playingRoom->y - 1;
                            playingRoom = rm;
                        }
                        break;
                    case ID_TRAP:
                        PulseEvent(hEvent[3]);
                        playingRoom->player->y--;

                        for (int i = 2; i < 5; i++) {
                            if (startRoom.elements[i].trap.id == ID_TRAP && startRoom.place[playingRoom->elements[i].trap.x][playingRoom->elements[i].trap.y] == playingRoom->place[playingRoom->player->x][playingRoom->player->y]) {
                                playingRoom->player->hp -= playingRoom->elements[i].trap.damage;
                                break;
                            }
                        }

                        break;
                    }

                    break;
                case 's':
                    switch (playingRoom->place[playingRoom->player->x][playingRoom->player->y + 1]) {
                    case 0:
                        playingRoom->player->y++;
                        break;
                    case ID_ENEMY:
                        PulseEvent(hEvent[0]);
                        break;
                    case ID_CHEST:
                        PulseEvent(hEvent[1]);
                        break;
                    case ID_DOOR:
                        if (playingRoom->solved == true) {
                            solvedRoom = true;
                            Room* rm;
                            if (map[playingRoom->x][playingRoom->y + 1] != nullptr) {
                                rm = map[playingRoom->x][playingRoom->y + 1];
                            }
                            else rm = new Room;
                            x = playingRoom->player->x;
                            y = playingRoom->player->y + 1;
                            rm->player = playingRoom->player;
                            rm->player->y = playingRoom->player->y - 7;
                            rm->x = playingRoom->x;
                            rm->y = playingRoom->y + 1;
                            playingRoom = rm;
                        }
                        break;
                    case ID_TRAP:
                        PulseEvent(hEvent[3]);
                        playingRoom->player->y++;

                        for (int i = 2; i < 5; i++) {
                            if (playingRoom->elements[i].trap.id == ID_TRAP && playingRoom->place[playingRoom->elements[i].trap.x][playingRoom->elements[i].trap.y] == playingRoom->place[playingRoom->player->x][playingRoom->player->y]) {
                                playingRoom->player->hp -= playingRoom->elements[i].trap.damage;
                                break;
                            }
                        }

                        break;
                    }

                    break;
                case 'a':
                    switch (playingRoom->place[playingRoom->player->x - 1][playingRoom->player->y]) {
                    case 0:
                        playingRoom->player->x--;
                        break;
                    case ID_ENEMY:
                        PulseEvent(hEvent[0]);
                        break;
                    case ID_CHEST:
                        PulseEvent(hEvent[1]);
                        break;
                    case ID_DOOR:
                        if (playingRoom->solved == true) {
                            solvedRoom = true;
                            Room *rm;
                            if (map[playingRoom->x - 1][playingRoom->y] != nullptr) {
                                rm = map[playingRoom->x - 1][playingRoom->y];
                            }
                            else rm = new Room;
                            x = playingRoom->player->x - 1;
                            y = playingRoom->player->y;
                            rm->player = playingRoom->player;
                            rm->player->x = playingRoom->player->x + 7;
                            rm->x = playingRoom->x - 1;
                            rm->y = playingRoom->y;
                            playingRoom = rm;
                        }
                        break;
                    case ID_TRAP:
                        PulseEvent(hEvent[3]);
                        playingRoom->player->x--;

                        for (int i = 2; i < 5; i++) {
                            if (playingRoom->elements[i].trap.id == ID_TRAP && playingRoom->place[playingRoom->elements[i].trap.x][playingRoom->elements[i].trap.y] == playingRoom->place[playingRoom->player->x][playingRoom->player->y]) {
                                playingRoom->player->hp -= playingRoom->elements[i].trap.damage;
                                break;
                            }
                        }

                        break;
                    }

                    break;
                case 'd':
                    switch (playingRoom->place[playingRoom->player->x + 1][playingRoom->player->y]) {
                    case 0:
                        playingRoom->player->x++;
                        break;
                    case ID_ENEMY:
                        PulseEvent(hEvent[0]);
                        break;
                    case ID_CHEST:
                        PulseEvent(hEvent[1]);
                        break;
                    case ID_DOOR:
                        if (playingRoom->solved == true) {
                            solvedRoom = true;
                            Room *rm;
                            if (map[playingRoom->x + 1][playingRoom->y] != nullptr) {
                                rm = map[playingRoom->x + 1][playingRoom->y];
                            }
                            else rm = new Room;
                            x = playingRoom->player->x + 1;
                            y = playingRoom->player->y;
                            rm->player = playingRoom->player;
                            rm->player->x = playingRoom->player->x - 7;
                            rm->x = playingRoom->x + 1;
                            rm->y = playingRoom->y;
                            playingRoom = rm;
                        }
                        break;
                    case ID_TRAP:
                        PulseEvent(hEvent[3]);
                        playingRoom->player->x++;

                        for (int i = 2; i < 5; i++) {
                            if (playingRoom->elements[i].trap.id == ID_TRAP && playingRoom->place[playingRoom->elements[i].trap.x][playingRoom->elements[i].trap.y] == playingRoom->place[playingRoom->player->x][playingRoom->player->y]) {
                                playingRoom->player->hp -= playingRoom->elements[i].trap.damage;
                                break;
                            }
                        }

                        break;
                    }

                    break;
                }
            }

            Sleep(200);


            system("cls");
        }
    }

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
