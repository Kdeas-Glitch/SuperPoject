#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <fstream>
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
#define CHANCE_FOR_ENEMY 70
#define CHANCE_FOR_CHEST 70
#define CHANCE_FOR_TRAP 75


CRITICAL_SECTION cs;
volatile int countRooms = 0;

struct Player {
    int x;
    int y;
    int x_room;
    int y_room;
    int hp = 300;
    int power = 40;
    int armor = 40; // резист от атаки при защите
    int intellect = 20; // шанс крита
    int countOfHeal = 100;
    int difficultyMultyplier = 1;
};


struct Enemy {
    int id;
    int x;
    int y;
    int health;
    int damage;
};

struct Trap {
    int id;
    int x;
    int y;
    int damage;
};

struct Chest {
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
    bool isLooted = false;
    int x;
    int y;

    bool solved = false;//пройдена ли комната

    bool up = false, down = false, left = false, right = false;//стороны где могут появиться проходы
    bool playerer;

    Player* player;

    int place[10][10] = {};//размеры комнаты

    Elements elements[5];//элементы которые могут появиться в комнате
};
HANDLE hMutex;
Room* map[10][10] = { nullptr };

void GenerateEnemy(Elements* element) {
    element->enemy.damage = 50;
    element->enemy.health = 500;
}

void GenerateTrap(Elements* element) {
    element->trap.damage = 30;
}

void GenerateChest(Elements* element) {

}

const std::string filename = "rooms.dat";
void saveRooms(Room* rooms[10][10]) {

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "File ERROR" << std::endl;
        return;
    }

    // Сохраняем только первые 10X10
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            Room& room = *rooms[i][j];
            if (rooms[i][j] != nullptr) {
                file.write(reinterpret_cast<const char*>(&room.x), sizeof(room.x));
                file.write(reinterpret_cast<const char*>(&room.y), sizeof(room.y));
                file.write(reinterpret_cast<const char*>(&room.solved), sizeof(room.solved));
                file.write(reinterpret_cast<const char*>(&room.down), sizeof(room.down));
                file.write(reinterpret_cast<const char*>(&room.right), sizeof(room.right));
                file.write(reinterpret_cast<const char*>(&room.up), sizeof(room.up));
                file.write(reinterpret_cast<const char*>(&room.left), sizeof(room.left));
                file.write(reinterpret_cast<const char*>(&room.playerer), sizeof(room.playerer));
            }
        }
    }
    file.close();
    ReleaseMutex(hMutex);
    Sleep(5);
    WaitForSingleObject(hMutex, INFINITY);
}

//макет генерации
void GenerateRoom(Room* room, Player* player, int x, int y) {

    room->id = countRooms;

    room->player = player;

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
    if (room->x - 1 >= 0 && map[room->x - 1][room->y] != nullptr) {
        if (map[room->x - 1][room->y]->right == true)
            room->left = true;
        else {
            room->left = false;
            canLeft = false;
            countAnotherRooms++;
        }
    }
    if (room->x + 1 < 10 && map[room->x + 1][room->y] != nullptr) {
        if (map[room->x + 1][room->y]->left == true)
            room->right = true;
        else {
            room->right = false;
            canRight = false;
            countAnotherRooms++;
        }
    }
    if (room->y - 1 >= 0 && map[room->x][room->y - 1] != nullptr) {
        if (map[room->x][room->y - 1]->down == true)
            room->up = true;
        else {
            room->up = false;
            canUp = false;
            countAnotherRooms++;
        }
    }
    if (room->y + 1 < 10 && map[room->x][room->y + 1] != nullptr) {
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


    //генерация приколов внутри комнаты

    for (int i = 0; i < 5; i++) {
        room->elements[i].chest.id = 0;
    }

    if (gen() % 100 <= CHANCE_FOR_ENEMY) {
        Enemy enemy;
        if (x == 0 || x == 9) {
            enemy.y = gen() % 5;
            if (enemy.y == 0) enemy.y++;
            enemy.x = gen() % 5;
            if (x == 9) enemy.x += 4;
            else if (enemy.x == 0) enemy.x++;
        }
        else if (y == 0 || y == 9) {
            enemy.x = gen() % 5;
            if (enemy.x == 0) enemy.x++;
            enemy.y = gen() % 5;
            if (y == 9) enemy.y += 4;
            else if (enemy.y == 0) enemy.y++;
        }
        enemy.id = ID_ENEMY;
        room->elements[0].enemy = enemy;
        GenerateEnemy(&room->elements[0]);
    }
    else room->solved = true;
    if (gen() % 100 <= CHANCE_FOR_CHEST) {
        Chest chest;
        if (x == 0 || x == 9) {
            chest.y = gen() % 5 + 4;
            chest.x = gen() % 5;
            if (x == 9) chest.x += 4;
            else if (chest.x == 0) chest.x++;
        }
        else if (y == 0 || y == 9) {
            chest.x = gen() % 5 + 4;
            chest.y = gen() % 5;
            if (y == 9) chest.y += 4;
            else if (chest.y == 0) chest.y++;
        }
        chest.id = ID_CHEST;
        room->elements[1].chest = chest;
        GenerateChest(&room->elements[1]);
    }
    for (int i = 2; i < 5; i++) {
        if (gen() % 100 <= CHANCE_FOR_TRAP) {
            Trap trap;
            trap.x = gen() % 10;
            trap.y = gen() % 10;
            if (trap.x == 0) trap.x++;
            else if (trap.x == 9) trap.x--;
            if (trap.y == 0) trap.y++;
            else if (trap.y == 9) trap.y--;

            if (!((trap.x == room->elements[0].enemy.x && trap.y == room->elements[0].enemy.y) || (trap.x == room->elements[1].chest.x && trap.y == room->elements[1].chest.y))) {
                trap.id = ID_TRAP;
                room->elements[i].trap = trap;
                GenerateTrap(&room->elements[i]);
            }
        }
    }

}

void PrintPlace(Room* room) {

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            room->place[i][j] = 0;
        }
    }


    //установка элементов

    bool onTrap = false;

    if (room->elements[0].enemy.id == ID_ENEMY && room->elements[0].enemy.health > 0)
        room->place[room->elements[0].enemy.x][room->elements[0].enemy.y] = ID_ENEMY;
    if (room->elements[1].chest.id == ID_CHEST && room->solved && !room->isLooted)
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
                std::cout << (char)128 << " ";
            else if (room->place[j][i] == ID_WALL)
                std::cout << (char)254 << " ";
            else if (room->place[j][i] == ID_DOOR) {
                if (room->solved)
                    std::cout << GREEN << (char)255 << WHITE << " ";
                else
                    std::cout << RED << (char)254 << WHITE << " ";
            }
            else if (room->place[j][i] == ID_ENEMY)
                std::cout << RED << (char)253 << WHITE << " ";
            else if (room->place[j][i] == ID_PLAYER) {
                if (onTrap)
                    std::cout << YELLOW << (char)253 << WHITE << " ";
                else
                    std::cout << (char)253 << " ";
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
    //std::cout << "enemy player - " << room->elements[0].enemy.health << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    /*for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (map[j][i] != nullptr) {
                std::cout << map[j][i]->id << " ";
            }
            else std::cout << 0 << " ";
        }
        std::cout << std::endl;
    }*/

}


DWORD WINAPI EnemyGameplay(LPVOID _room) {

    Room* room = (Room*)_room;
    Enemy* enemy = &room->elements[0].enemy;
    Player* player = room->player;

    HANDLE hEvent;
    hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"FightEvent");
    if (hEvent == NULL)
        return GetLastError();

    HANDLE hEndEvent;
    hEndEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"EndFightEvent");
    if (hEndEvent == NULL)
        return GetLastError();

    while (enemy->health > 0) {
        WaitForSingleObject(hEndEvent, INFINITE);
        
        if (room->player->hp <= 0) {
            break;
        }
        EnterCriticalSection(&cs);
        if (player->x < enemy->x) {
            if (player->x != enemy->x - 1 || player->y != enemy->y)
                enemy->x--;
            else {
                PulseEvent(hEvent);
                LeaveCriticalSection(&cs);
                continue;
            }
        }
        else if (player->x > enemy->x) {
            if (player->x != enemy->x + 1 || player->y != enemy->y)
                enemy->x++;
            else {
                PulseEvent(hEvent);
                LeaveCriticalSection(&cs);
                continue;
            }
        }
        if (player->y < enemy->y) {
            if (player->x != enemy->x || player->y != enemy->y - 1)
                enemy->y--;
            else {
                PulseEvent(hEvent);
                LeaveCriticalSection(&cs);
                continue;
            }
        }
        else if (player->y > enemy->y) {
            if (player->x != enemy->x || player->y != enemy->y + 1)
                enemy->y++;
            else {
                PulseEvent(hEvent);
                LeaveCriticalSection(&cs);
                continue;
            }
        }
        LeaveCriticalSection(&cs);
        Sleep(700);
    }

    room->solved = true;

    return 0;
}

DWORD WINAPI EnemyFight(LPVOID _room) {
    HANDLE hEvent;
    hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"FightEvent");
    if (hEvent == NULL)
        return GetLastError();

    WaitForSingleObject(hEvent, INFINITE);

    HANDLE hEndEvent;
    hEndEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"EndFightEvent");
    if (hEndEvent == NULL)
        return GetLastError();

    ResetEvent(hEndEvent);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    wchar_t fight_process[] = L"C:\\Users\\Leshu\\Desktop\\project\\full\\SuperPoject\\x64\\Debug\\Fight.exe ";
    

    Room* room = (Room*)_room;
    std::ofstream data("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла // ЗАПИСЬ
    if (data.is_open()) { // запись

        data << room->player->hp << std::endl;
        data << room->player->power << std::endl;
        data << room->player->armor << std::endl;
        data << room->player->intellect << std::endl;
        data << room->player->countOfHeal << std::endl;
        data << room->player->difficultyMultyplier << std::endl;
        
        data.close();
    }
    if (!CreateProcess(fight_process, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        return GetLastError();
    }

    WaitForSingleObject(hEndEvent, INFINITE);

    std::ifstream file("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла ЧТЕНИЕ

    if (file.is_open()) { // чтение

        file >> room->player->hp;
        file >> room->player->power;
        file >> room->player->armor;
        file >> room->player->intellect;
        file >> room->player->countOfHeal;
        file >> room->player->difficultyMultyplier;

        file.close();
    }
    if (room->player->hp > 0) {
        room->elements[0].enemy.health = 0;
    }
    

    
}

DWORD WINAPI ChestLoot(LPVOID _room) {
    HANDLE hEvent;
    hEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"ChestEvent");
    if (hEvent == NULL)
        return GetLastError();

    WaitForSingleObject(hEvent, INFINITE);

    HANDLE hEndEvent;
    hEndEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"EndChestEvent");
    if (hEndEvent == NULL)
        return GetLastError();

    ResetEvent(hEndEvent);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    wchar_t chest_process[] = L"C:\\Users\\Leshu\\Desktop\\project\\full\\SuperPoject\\x64\\Debug\\Chest.exe ";
   

    Room* room = (Room*)_room;

    std::ofstream data("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла // ЗАПИСЬ
    if (data.is_open()) { // запись

        data << room->player->hp << std::endl;
        data << room->player->power << std::endl;
        data << room->player->armor << std::endl;
        data << room->player->intellect << std::endl;
        data << room->player->countOfHeal << std::endl;
        data << room->player->difficultyMultyplier << std::endl;

        data.close();
    }
    if (!CreateProcess(chest_process, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        return GetLastError();
    }

    WaitForSingleObject(hEndEvent, INFINITE);

    std::ifstream file("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла ЧТЕНИЕ

    if (file.is_open()) { // чтение

        file >> room->player->hp;
        file >> room->player->power;
        file >> room->player->armor;
        file >> room->player->intellect;
        file >> room->player->countOfHeal;
        file >> room->player->difficultyMultyplier;

        file.close();
    }
    
}

void StartGenerate(Room* startRoom, Player* player) {
    startRoom->player = player;

    startRoom->player->x = 4;
    startRoom->player->y = 4;

    startRoom->x = 4;
    startRoom->y = 4;

    startRoom->id = 1;
    countRooms++;

    map[startRoom->x][startRoom->y] = startRoom;

    GenerateRoom(startRoom, startRoom->player, 4, 9);
    startRoom->left = true;
    startRoom->right = true;
    startRoom->down = true;
    startRoom->up = true;
    startRoom->elements[0].enemy.id = 0;
    startRoom->solved = true;
    if (startRoom->elements[1].chest.id == ID_CHEST) {
        startRoom->elements[1].chest.x = 8;
        startRoom->elements[1].chest.y = 8;
    }
}

int main()
{
    setlocale(LC_ALL, ".UTF16");
    bool MapisOpen = false;
    hMutex = CreateMutex(NULL, FALSE, L"FileMutex");
    if (hMutex == NULL)
    {
        return  GetLastError();
    }
    WaitForSingleObject(hMutex, INFINITY);
    PROCESS_INFORMATION ClientApp[100];
    int countmap = 0;
    STARTUPINFO siPrint;

    

    STARTUPINFO siClient;
    bool isFinal = false;
    std::ofstream sbros("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\final.txt");
    if (sbros.is_open()) {
        sbros << isFinal << std::endl;
    }

    ZeroMemory(&siClient, sizeof(siClient));
    ZeroMemory(&siPrint, sizeof(siPrint));

    wchar_t mapping[255] = L"C:\\Users\\Leshu\\Desktop\\project\\full\\SuperPoject\\x64\\Debug\\Mapping.exe ";

    HANDLE hEvent[5];
    hEvent[0] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"FightEvent");
    hEvent[1] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"ChestEvent");
    hEvent[2] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)"MapEvent");
    hEvent[3] = CreateEvent(NULL, TRUE, TRUE, (LPCWSTR)"EndFightEvent");
    hEvent[4] = CreateEvent(NULL, TRUE, TRUE, (LPCWSTR)"EndChestEvent");
    if (hEvent[0] == NULL || hEvent[1] == NULL || hEvent[2] == NULL || hEvent[3] == NULL || hEvent[4] == NULL)
        return GetLastError();

    Room startRoom;
    Player player;
    std::ofstream data("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла // ЗАПИСЬ
    if (data.is_open()) { // запись

        data << player.hp << std::endl;
        data << player.power << std::endl;
        data << player.armor << std::endl;
        data << player.intellect << std::endl;
        data << player.countOfHeal << std::endl;
        data << player.difficultyMultyplier << std::endl;
        

        data.close();
    }
    StartGenerate(&startRoom, &player);

    /*for (int i = 0; i < 256; i++) {
        char s = i;
        std::cout << s << " - " << i << std::endl;
    }*/
    int x = 0, y = 0;
    Room* playingRoom = &startRoom;
    InitializeCriticalSection(&cs);



    std::cout << "            SUPERPOJECT               " << std::endl;
    std::cout << "        Choose Difficulty:            " << std::endl;
    std::cout << "              1-Easy                  " << std::endl;
    std::cout << "              2-Middle                " << std::endl;
    std::cout << "              3-Hard                  " << std::endl;
    int difficulty = _getch();

    bool isBoss = false;
    
    
    if (difficulty == '1' || difficulty=='2'||difficulty=='3') {
        while (player.hp > 0 && !isFinal) { 

            if (playingRoom->id == 0)
                GenerateRoom(playingRoom, &player, x, y);
            if (playingRoom->id >= 5) { // БОСС
                isBoss = true;
                std::ofstream inBoss("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\Boss.txt");
                if (inBoss.is_open()) {
                    inBoss << isBoss;
                    inBoss.close(); 
                }
               
                
            }
            else {
                isBoss = false;
                std::ofstream inBoss("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\Boss.txt");
                if (inBoss.is_open()) {
                    inBoss << isBoss;
                    inBoss.close();
                }
            }
            saveRooms(map);
            HANDLE hThreads[2];
            DWORD IDThreads[2];
            DWORD IDChest;
            HANDLE hChest[1];
            bool hasEnemy = false;
            bool hasChest = false;
            if (playingRoom->elements[0].enemy.id == ID_ENEMY) {
                hasEnemy = true;
                hThreads[0] = CreateThread(NULL, 0, EnemyGameplay, playingRoom, NULL, &IDThreads[0]);
                if (hThreads[0] == NULL)
                    return GetLastError();

                hThreads[1] = CreateThread(NULL, 0, EnemyFight, playingRoom, NULL, &IDThreads[1]);
                if (hThreads[1] == NULL)
                    return GetLastError();

            }
            if (playingRoom->elements[1].chest.id == ID_CHEST) {
                hasChest = true;
                hChest[0] = CreateThread(NULL, 0, ChestLoot, playingRoom, NULL, &IDChest);
                if (hChest[0] == NULL) {
                    return GetLastError();
                }
            }
            bool solvedRoom = false;
            while (solvedRoom != true) {
                PrintPlace(playingRoom);

                WaitForSingleObject(hEvent[3], INFINITE);
                WaitForSingleObject(hEvent[4], INFINITE);
                std::ifstream file("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\data.txt"); // Открытие файла ЧТЕНИЕ

                if (file.is_open()) { // чтение

                    file >> playingRoom->player->hp;
                    file >> playingRoom->player->power;
                    file >> playingRoom->player->armor;
                    file >> playingRoom->player->intellect;
                    file >> playingRoom->player->countOfHeal;
                    file >> playingRoom->player->difficultyMultyplier;
                   

                    file.close();
                }
                
                std::ifstream finalGame("C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\final.txt"); // Открытие файла

                if (finalGame.is_open()) { // чтение

                    finalGame >> isFinal;
                    finalGame.close();
                }

                if (isFinal) {
                    break;
                }

                if (player.hp <= 0)
                {
                    break;
                }

                if (_kbhit() == 1) {
                    EnterCriticalSection(&cs);
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
                            playingRoom->isLooted = true;
                            break;
                        case ID_DOOR:
                            if (playingRoom->solved == true) {
                                solvedRoom = true;
                                Room* rm;
                                if (map[playingRoom->x][playingRoom->y - 1] != nullptr) {
                                    rm = map[playingRoom->x][playingRoom->y - 1];
                                }
                                else {
                                    rm = new Room;
                                    countRooms++;
                                }
                                x = playingRoom->player->x;
                                y = playingRoom->player->y - 1;
                                rm->player = playingRoom->player;
                                rm->player->y = playingRoom->player->y + 7;
                                rm->x = playingRoom->x;
                                rm->y = playingRoom->y - 1;
                                playingRoom->playerer = false;
                                rm->playerer = true;
                                playingRoom = rm;
                            }
                            break;
                        case ID_TRAP:
                            playingRoom->player->y--;

                            for (int i = 2; i < 5; i++) {
                                if (playingRoom->elements[i].trap.id == ID_TRAP && playingRoom->place[playingRoom->elements[i].trap.x][playingRoom->elements[i].trap.y] == playingRoom->place[playingRoom->player->x][playingRoom->player->y]) {
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
                            playingRoom->isLooted = true;
                            break;
                        case ID_DOOR:
                            if (playingRoom->solved == true) {
                                solvedRoom = true;
                                Room* rm;
                                if (map[playingRoom->x][playingRoom->y + 1] != nullptr) {
                                    rm = map[playingRoom->x][playingRoom->y + 1];
                                }
                                else {
                                    rm = new Room;
                                    countRooms++;
                                }
                                x = playingRoom->player->x;
                                y = playingRoom->player->y + 1;
                                rm->player = playingRoom->player;
                                rm->player->y = playingRoom->player->y - 7;
                                rm->x = playingRoom->x;
                                rm->y = playingRoom->y + 1;
                                playingRoom->playerer = false;
                                rm->playerer = true;
                                playingRoom = rm;
                            }
                            break;
                        case ID_TRAP:
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
                            playingRoom->isLooted = true;
                            break;
                        case ID_DOOR:
                            if (playingRoom->solved == true) {
                                solvedRoom = true;
                                Room* rm;
                                if (map[playingRoom->x - 1][playingRoom->y] != nullptr) {
                                    rm = map[playingRoom->x - 1][playingRoom->y];
                                }
                                else {
                                    rm = new Room;
                                    countRooms++;
                                }
                                x = playingRoom->player->x - 1;
                                y = playingRoom->player->y;
                                rm->player = playingRoom->player;
                                rm->player->x = playingRoom->player->x + 7;
                                rm->x = playingRoom->x - 1;
                                rm->y = playingRoom->y;
                                playingRoom->playerer = false;
                                rm->playerer = true;
                                playingRoom = rm;
                            }
                            break;
                        case ID_TRAP:
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
                            playingRoom->isLooted = true;
                            break;
                        case ID_DOOR:
                            if (playingRoom->solved == true) {
                                solvedRoom = true;
                                Room* rm;
                                if (map[playingRoom->x + 1][playingRoom->y] != nullptr) {
                                    rm = map[playingRoom->x + 1][playingRoom->y];
                                }
                                else {
                                    rm = new Room;
                                    countRooms++;
                                }
                                x = playingRoom->player->x + 1;
                                y = playingRoom->player->y;
                                rm->player = playingRoom->player;
                                rm->player->x = playingRoom->player->x - 7;
                                rm->x = playingRoom->x + 1;
                                rm->y = playingRoom->y;
                                playingRoom->playerer = false;
                                rm->playerer = true;
                                playingRoom = rm;
                            }
                            break;
                        case ID_TRAP:
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
                    case 'm':
                        if (!MapisOpen) {
                            if (!CreateProcess(NULL, mapping, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &siClient, &ClientApp[countmap])) {
                                std::cout << "Child process is not Created";
                                return 0;

                            }
                            MapisOpen = true;
                            countmap++;
                        }
                        else {
                            TerminateProcess(ClientApp[countmap - 1].hProcess, 0);
                            CloseHandle(ClientApp[countmap - 1].hProcess);
                            CloseHandle(ClientApp[countmap - 1].hThread);
                            countmap--;
                            MapisOpen = false;
                        }
                        break;
                    }
                    LeaveCriticalSection(&cs);
                }
                saveRooms(map);
                Sleep(200);


                system("cls");
            }
            if (hasEnemy) {
                CloseHandle(hThreads[0]);
                CloseHandle(hThreads[1]);
            }
            if (hasChest) {
                CloseHandle(hChest[0]);
            }
        }
    }
    else {
        std::cout << "        БЫЛО СКАЗАННО 1-3!!!!           " << std::endl;
    }
    system("cls");
    if (player.hp <= 0) {

        std::cout << "you lose etc...";
        _getch();
    }
    else {
        std::cout << "you win etc...";
        _getch();
    }
   
    DeleteCriticalSection(&cs);
    CloseHandle(hEvent[0]);
    CloseHandle(hEvent[1]);
    CloseHandle(hEvent[2]);
    CloseHandle(hEvent[3]);
}