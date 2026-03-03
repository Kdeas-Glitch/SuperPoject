// game_pract_sp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <conio.h>

#define ID_PLAYER 1
#define ID_ENEMY 2
#define ID_CHEST 3
#define ID_TRAP 4
#define ID_DOOR 5
#define RED "\033[31m"
#define WHITE "\033[0m"

struct Player {
    int x;
    int y;
};

struct Enemy{
    int id;
    int x;
    int y;
};

struct Trap{
    int id;
    int x;
    int y;
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
    int x;
    int y;

    bool solved;//пройдена ли комната

    bool up,down,left,right;//стороны где могут появиться проходы


    int place[10][10] = {};//размеры комнаты

    Elements elements[5];//элементы которые могут появиться в комнате

    //Elements* elements[5];
};

//макет генерации
void GenerateRoom(Room* room, int x, int y) {
    room->x = 1;
    room->y = 2;

    //присваивание двери через которубю вошли
    if (y == 0)
        room->down = true;
    if (y == 9)
        room->up = true;
    if (x == 0)
        room->right = true;
    if (x == 9)
        room->left = true;

    room->right = false;
    room->left = true;
    room->up = true;

    //функция вызова шансов 
    Enemy enemy;
    enemy.x = 1;
    enemy.y = 2;
    enemy.id = ID_ENEMY;

    Chest chest;
    chest.x = 8;
    chest.y = 2;
    chest.id = ID_CHEST;

    Trap trap;
    trap.x = 5;
    trap.y = 4;
    trap.id = ID_TRAP;

    room->elements[0].enemy = enemy;
    room->elements[1].chest = chest;
    room->elements[2].trap = trap;
}

void PrintPlace(Room* room, Player player) {

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            room->place[i][j] = 0;
        }
    }

    room->place[player.x][player.y] = ID_PLAYER;

    //установка элементов
    for (int i = 0; i < 5; i++) {
        if (room->elements[i].chest.id == ID_CHEST) {
            room->place[room->elements[i].chest.x][room->elements[i].chest.y] = ID_CHEST;
        }
        else if (room->elements[i].enemy.id == ID_ENEMY) {
            room->place[room->elements[i].enemy.x][room->elements[i].enemy.y] = ID_ENEMY;
        }
        else if (room->elements[i].trap.id == ID_TRAP) {
            room->place[room->elements[i].trap.x][room->elements[i].trap.y] = ID_TRAP;
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
            if (i == 0 || i == 9 || j == 0 || j == 9) {

                if (room->place[j][i] == ID_DOOR)
                    std::cout << RED << (char)254 << " " << WHITE;
                else
                    std::cout << (char)254 << " ";
            }
            else {
                if (room->place[j][i] == 0)
                    std::cout << (char)255 << " ";
                else
                    std::cout << room->place[j][i] << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main()
{
    Room rm;

    Player player;

    player.x = 4;
    player.y = 8;

    GenerateRoom(&rm, 4,0);

    //for (int i = 0; i < 256; i++) {
    //    char s = i;
    //    std::cout << s << " ";
    //}

    while (true) {
        PrintPlace(&rm, player);

        if (_kbhit() == 1) {
            switch (_getch()) {
            case 'w':
                player.y--;
                break;
            case 's':
                player.y++;
                break;
            case 'a':
                player.x--;
                break;
            case 'd':
                player.x++;
                break;
            }
        }

        Sleep(200);


        system("cls");
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
