#include <iostream>
#include <windows.h>
#include <fstream>
//Цвета при помощи кодов
#define WHITE "\033[0m"
#define GREEN "\033[35m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define COLOR "\033[36m"

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
struct Player {
    int x;
    int y;

    int x_room;
    int y_room;

    int hp = 1000;
};
struct Room {
    int X = -1;
    int Y;

    bool solved;

    bool Down;
    bool Right;
    bool Top;
    bool Left;

    Player* player;

    int place[10][10] = {};

    Elements elements[5];
};



const char* get_color(bool solved) {//Цвета для всех комнат кроме краёв
    if (solved) {
        return "\033[32m"; // Зеленый
    }
    else {
        return "\033[33m"; // Желтый
    }
}
const char* get_color_other(int solved) {//Цвета для краёв
    if (solved == 0) {
        return "\033[32m"; // Зеленый
    }
    else if (solved == 1) {
        return "\033[33m"; // Желтый
    }
    else {
        return "\033[0m"; // Белый
    }
}
const std::string filename = "rooms.dat";

void loadRooms(Room rooms[11][10]) {//чтение из файла
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error With File" << std::endl;
        return;
    }

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            Room& room = rooms[i][j];

            file.read(reinterpret_cast<char*>(&room.X), sizeof(room.X));//Запись нужных элементов
            file.read(reinterpret_cast<char*>(&room.Y), sizeof(room.Y));
            file.read(reinterpret_cast<char*>(&room.solved), sizeof(room.solved));
            file.read(reinterpret_cast<char*>(&room.Down), sizeof(room.Down));
            file.read(reinterpret_cast<char*>(&room.Right), sizeof(room.Right));
            file.read(reinterpret_cast<char*>(&room.Top), sizeof(room.Top));
            file.read(reinterpret_cast<char*>(&room.Left), sizeof(room.Left));
        }
    }
    file.close();
}

int main()
{
    Player pl{ 1,4 };
    Room rooms[11][10];
    Room nt{ 5,6 };
    nt.X = 6;
    nt.Left = true;
    nt.Down = true;
    nt.Right = true;
    nt.Top = true;
    nt.solved = true;
    //for (int i = 0; i < 11; i++)
    //    rooms[i][1] = nt;
    //rooms[4][5] = nt;
    //nt.solved = false;
    rooms[4][4] = nt;
    rooms[1][1] = nt;
    rooms[2][2] = nt;
    rooms[3][3] = nt;
    rooms[5][5] = nt;
    bool center = true;
    bool middle = false;
    std::cout << std::endl;
    HANDLE hMutex = CreateMutex(NULL, FALSE, L"FileMutex");
    if (hMutex == NULL)
    {
        return  GetLastError();
    }

    bool left = true;
    while (true) {
        DWORD res = WaitForSingleObject(hMutex, INFINITY);
        loadRooms(rooms);
        for (int i = 0; i <= 20; i++) {//Проход по У

            if (rooms[i / 2][0].X != -1) {//Проверка на существование комнаты если да то вывести с цветом
                std::cout << get_color_other(!rooms[i / 2][0].solved) << "|" << WHITE;
            }
            else {
                std::cout << get_color_other(3) << "|" << WHITE;//Без цвета
            }

            for (int j = 1; j < 60; j++) {//Проход по Х
                if (rooms[i / 2][j / 6].X != -1) {//Проверка на существование комнаты если

                    if (j % 6 == 0) {//Проверка на то что это край комнаты
                        if (i % 2 == 1) {//Проверка на то что это средина по вертикали
                            middle = true;
                        }
                        else {
                            middle = false;
                        }
                        left = true;
                        if (rooms[i / 2][j / 6].Left && left && middle) {//Проверка на то что есть проход слева(.Left) и середина(middle) и то что стенка левая(left)
                            std::cout << get_color(rooms[i / 2][j / 6].solved && rooms[i / 2][(j - 1) / 6].solved) << "=" << WHITE;//если комната которая рисуется или комната слева не решена то рисуется жёлтое равно
                        }
                        else if (rooms[i / 2][j / 6].Right && left && middle) {//Не работает не трогать (По идее должен вырисовывать проход справа)
                            std::cout << get_color(true) << "=" << WHITE;
                        }
                        else
                        {
                            bool current_room_solved = rooms[i / 2][j / 6].X != -1 && rooms[i / 2][j / 6].solved;//Проверка на то решена ли комната или нет
                            bool left_room_solved = (j > 0) && rooms[i / 2][(j - 1) / 6].X != -1 && rooms[i / 2][(j - 1) / 6].solved;//проверка на то что слева комната решена

                            // Если слева есть комната и она решена
                            if ((rooms[i / 2][j / 6].X != -1 && current_room_solved) ||
                                (j > 0 && rooms[i / 2][(j - 1) / 6].X != -1 && left_room_solved)) {
                                std::cout << get_color(true) << "|" << WHITE; // Зеленая стена
                            }
                            else {
                                std::cout << get_color(current_room_solved || left_room_solved) << "|" << WHITE;//Должен вывести жёлтым
                            }
                        }
                    }
                    else {
                        if (i % 2 == 0) {//Проверка на то что потолок
                            if (rooms[(i - 1) / 2][(j - 1) / 6].X != -1) {//Проверка на то что комната существует
                                if ((j - 3) % 6 == 0) {//Проверка на середину комнаты
                                    middle = true;
                                }
                                else {
                                    middle = false;
                                }
                                if (middle && rooms[(i - 1) / 2][(j) / 6].X != -1 && rooms[(i - 1) / 2][(j) / 6].Down && i > 1 && i < 20)//Проверка на то что комната существует и есть проход снизу
                                    std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)206 << WHITE;//Нарисовать проход
                                else {
                                    if(middle && rooms[(i) / 2][(j) / 6].X != -1 && rooms[(i) / 2][(j) / 6].Top && i > 1 && i < 20)
                                    std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)206 << WHITE;//Нарисовать _
                                    else
                                        std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)196 << WHITE;
                                }
                            }
                            else
                            {
                                if ((j - 3) % 6 == 0) {//Проверка на середину комнаты
                                    middle = true;
                                }
                                else {
                                    middle = false;
                                }
                                if((middle && rooms[(i) / 2][(j) / 6].X != -1 && rooms[(i) / 2][(j) / 6].Top && i > 1 && i < 20))
                                    std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)206 << WHITE;
                                else
                                std::cout << get_color(rooms[i / 2][j / 6].solved) << (char)196 << WHITE;//Если конмнаты нет то _
                                //std::cout << (char)196;
                            }
                        }
                        else {
                            if ((j - 3) % 6 == 0) {//Проверка на то что середина
                                middle = true;
                            }
                            else {
                                middle = false;
                            }
                            if (middle && pl.y_room == i / 2 && pl.x_room == j / 6)//Если Середина и координаты комнаты совпадают с координатами игрока
                                std::cout << COLOR << "*" << WHITE;//Игрок
                            else
                                std::cout << " " << WHITE;
                        }
                    }
                }
                else {
                    if (i % 2 == 1) {//Проверка на середину
                        middle = true;
                    }
                    else {
                        middle = false;
                    }
                    if (j % 6 == 0) {//Проверка на стену
                        left = true;
                    }
                    else
                    {
                        left = false;
                    }
                    if ((rooms[(i) / 2][(j - 1) / 6].X != -1 || rooms[(i - 1) / 2][(j) / 6].X != -1 || rooms[(i - 1) / 2][(j - 1) / 6].X != -1) && j % 6 == 0) {//Проверка на существование комнаты слева или сверху или Слева-Сверху и стена ли
                        if (rooms[i / 2][j / 6].Right && left && middle) {//dd(Не работает) Если есть проход справа то вывести равно
                            std::cout << get_color(true) << "=" << WHITE;
                        }
                        else
                        {
                            if (rooms[i / 2][(j - 1) / 6].Right && !left && middle) {//(Не работает) если есть проход у комнаты слева справа то равно
                                std::cout << get_color(true) << "=" << WHITE;
                            }
                            else {
                                /*std::cout << get_color(rooms[(i) / 2][(j-1) / 6].solved&& rooms[(i-1) / 2][(j - 1) / 6].solved&& (rooms[(i - 1) / 2][(j) / 6].solved)) << "|" << WHITE;*/
                                //bool top_room_solved = (i > 0) && rooms[(i - 1) / 2][(j - 1) / 6].X != -1 && rooms[(i - 1) / 2][(j - 1) / 6].solved;
                                //bool bottom_room_solved = rooms[i / 2][(j - 1) / 6].X != -1 && rooms[i / 2][(j - 1) / 6].solved;

                                //// Стена зеленая, если хотя бы одна из комнат решена (независимо от проходов)
                                //if (top_room_solved || bottom_room_solved) {
                                //    std::cout << get_color(true) << "|" << WHITE; // Зеленая стена
                                //}
                                //else {
                                //    std::cout << get_color(false) << "|" << WHITE; // Желтая стена
                                //}

                                int left_col = (j - 1) / 6;
                                int right_col = j / 6;

                                // Проверяем комнаты слева (верхнюю и нижнюю)
                                bool top_left_solved = (i > 0) && rooms[(i - 1) / 2][left_col].X != -1 && rooms[(i - 1) / 2][left_col].solved;
                                bool bottom_left_solved = rooms[i / 2][left_col].X != -1 && rooms[i / 2][left_col].solved;

                                // Проверяем комнаты справа (верхнюю и нижнюю)
                                bool top_right_solved = (i > 0) && rooms[(i - 1) / 2][right_col].X != -1 && rooms[(i - 1) / 2][right_col].solved;
                                bool bottom_right_solved = rooms[i / 2][right_col].X != -1 && rooms[i / 2][right_col].solved;

                                // Стена зеленая, если ЛЮБАЯ из этих четырех комнат решена
                                if (top_left_solved || bottom_left_solved || top_right_solved || bottom_right_solved) {
                                    std::cout << get_color(true) << "|" << WHITE; // Зеленая
                                }
                                else {
                                    std::cout << get_color(false) << "|" << WHITE; // Желтая
                                }

                            }
                        }
                    }
                    else {
                        if (j % 6 == 0) {//Если стена
                            std::cout << "|";//вывести стену
                        }
                        else {
                            if (rooms[(i - 1) / 2][(j - 1) / 6].X != -1 && i % 2 == 0) {//Если комната слева-сверху существует и стена
                                if ((j - 3) % 6 == 0) {//Проверка на середину
                                    middle = true;
                                }
                                else {
                                    middle = false;
                                }
                                if (middle && rooms[(i - 1) / 2][(j) / 6].X != -1 && rooms[(i - 1) / 2][(j) / 6].Down)//Если середина и комната существует  и есть проход вниз то отобразить проход вниз
                                    std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)206 << WHITE;
                                else
                                    std::cout << get_color(rooms[(i - 1) / 2][(j - 1) / 6].solved) << (char)196 << WHITE;//Вывести просто _
                            }
                            else if (i % 2 == 0)
                            {
                                std::cout << (char)196;//Вывести просто _
                            }
                            else
                                std::cout << " ";
                        }
                    }
                }
            }
            if (rooms[i / 2][9].X != -1) {//Проверка на то что есть комната на краю
                std::cout << get_color_other(!rooms[i / 2][9].solved) << "|" << WHITE << std::endl;//Есть то вывести цветную стену
            }
            else {
                std::cout << get_color_other(3) << "|" << WHITE << std::endl;//Вывести белым
            }
        }
        Sleep(200);
        system("cls");
    }

    /* for (int i = 0; i < 256; i++) {
         char q = i;
         std::cout <<q<<i;
     }*/
}