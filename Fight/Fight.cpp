#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <iomanip>

#define URL_DATA "data.txt"
#define URL_BOSS "boss.txt"
#define URL_FINAL "final.txt"

using namespace std;
const string SLIME[] = {
"                         **************                                 ",
"                     //*******************\\\                             ",
"                    /**                   **\\*                          ",
"                   *//                        **\\                       ",
"                  *//                            \\*                     ",
"                 */      |||_         *||_        \\*                    ",
"                */       ||||    ||___|||||^       \\*                   ",
"               */         *\||||||||||/*             \\*                 ",
"              */*           ********/*               \\*                 ",
"              /*          ***      ***               \\*                 ",
"             /*          ||||\___/||||                \\*             ",
"             |           \|||||||||/                  \\*             ",
"            /|                                       ||             ",
"            ||             **____***                 ||             ",
"            ||             _|***\\|_                  ||             ",
"           |||             ||    ||                  ||             ",
"           |||             ||_  _||                  |||",
"           |||             *\\  ||*                   |||             ",
"           |||\             \\|\\/|/                  /|||             ",
"            |||\             **                     /|||             ",
"             \|||||\                        ////|||||//             ",
"              \|||||||||________________/|||||||||||//             ",
"               \|||||||||||||||||||||||||||||||////",
"                  *\\\||||||||||||||||||||||/*",
};
const string CYCLOP[] = {
"                       _______________                    ",
"                   .-'               '-.                  ",
"                 .'                     '.                ",
"                /                         \\               ",
"               |          _____            |              ",
"               |        .'     '.          |              ",
"               |       /   _     \\         |              ",
"               |      |   (_)     |        |             ",
"               |      |           |        |             ",
"               |       \\         /         |             ",
"               |        '._   _.'          |             ",
"               |           '-'             |             ",
"               |      _____________        |             ",
"               |    / | | | | | | | \\      |             ",
"               |   |  |_|_|_|_|_|_|  |     |             ",
"               |    \\_______________/      |             ",
"                \\                         /              ",
"                 '.                      .'               ",
"                   '-.              ....-'                ",
"                      '-..........-'                      ",
};
const string ANTMAN[] = {
"                                                                      ",
"                                                                      ",
"                                                                      ",
"                       ______________                                 ",
"                   .-'               '-.                              ",
"                 .'    _           _    '.                            ",
"                /    .' '.       .' '.    \\                           ",
"               |    /      \\   /      \\   |                      ",
"               |   |   O    | |    O   |  |                      ",
"               |    \\      /   \\      /   |                      ",
"                \\    '.__.'     '.__.'   /                      ",
"                 '.         ___         .'                        ",
"                   '-.__   /   \\   __.-'              ",
"                       \\ |     | /                   ",
"                        \\|     |/                    ",
"                       ___|     |___                  ",
"                     /   |     |   \\                 ",
"                    /    |     |    \\                ",
"                   /     |     |     \\               ",
"                  /      |     |      \\              ",
"                 '       |     |        '             ",
};
const string BOSS[] = {
    "              @@@@@@@@@@@@@@@              ",
    "           @@%%%%%%%%%%%%%%%@@             ",
    "         @@%%###############%%@@           ",
    "        @%##(((((((((((((((((##%@          ",
    "       @%#((((((((((((((((((((#%@          ",
    "       @#((((((((((((((((((((((#@          ",
    "       @#((((((((((((((((((((((#@          ",
    "       @%#((((((((((((((((((((#%@          ",
    "        @%##(((((((((((((((##%@            ",
    "         @@%%###############%%@@           ",
    "           @@%%%%%%%%%%%%%%%@@             ",
    "              @@@@@@@@@@@@@@@              ",
    "               @@         @@               ",
    "              @@@@       @@@@              ",
    "             @@@@@@     @@@@@@             ",
    "            @@@@@@@@   @@@@@@@@            ",
    "           @@@@@@@@@@ @@@@@@@@@@           ",
    "          @@@@@@@@@@@@@@@@@@@@@@@          ",
    "         @@@@@@@@@@@@@@@@@@@@@@@@@         ",
    "        @@@@@@@@@@@@@@@@@@@@@@@@@@@        "
};
struct Player
{
    int hp;
    int power;
    int armor; // резист от атаки при защите
    int intellect; // шанс крита
    int countOfHeal;
    int difficultyMultyplier;
};
struct Enemy
{
    string name = "";
    int hp = 100;
    int power = 13;
    int armor = 7;
    int chanceOfCrit = 10;
};

struct Data { // структура для передачи в поток Fight
    Enemy* en;
    Player* pl;
};
bool isBoss = false;
void UpdateInterface(Enemy& enemy, Player& player, string linesPlayer[]) {
    system("cls");


    string lines[6] = {
        "ВРАГ: " + enemy.name,
        "Характеристики врага:",
        "HP: " + to_string(enemy.hp),
        "Сила: " + to_string(enemy.power),
        "Броня: " + to_string(enemy.armor),
        "Шанс крита: " + to_string(enemy.chanceOfCrit)
    };


    string updatedLinesPlayer[7] = {
        "ИГРОК",
        "Характеристики игрока:",
        "HP: " + to_string(player.hp),
        "Сила: " + to_string(player.power),
        "Броня: " + to_string(player.armor),
        "Интеллект: " + to_string(player.intellect),
        "Зелья здоровья(шт): " + to_string(player.countOfHeal),

    };

    int count = 6;
    int countPlayer = 7;
    int maxLines = 23;

    if (enemy.name == "Слайм") {
        for (int i = 0; i < maxLines; i++) {
            if (i < 23) {
                cout << setw(40) << left << SLIME[i];
            }
            else {
                cout << setw(40) << left << " ";
            }

            if (i < count) {
                cout << setw(21) << left << lines[i];
            }
            else {
                cout << setw(21) << left << " ";
            }
            cout << "    ";

            if (i < countPlayer) {
                cout << setw(20) << left << updatedLinesPlayer[i];
            }
            else {
                cout << setw(20) << left << " ";
            }

            cout << endl;
        }
    }
    else if (enemy.name == "Циклоп") {
        for (int i = 0; i < maxLines; i++) {
            if (i < 20) {
                cout << setw(20) << left << CYCLOP[i];
            }
            else {
                cout << setw(20) << left << " ";
            }

            if (i < count) {
                cout << setw(25) << left << lines[i];
            }
            else {
                cout << setw(25) << left << " ";
            }

            if (i < countPlayer) {
                cout << setw(20) << left << updatedLinesPlayer[i];
            }
            else {
                cout << setw(20) << left << " ";
            }

            cout << endl;
        }
    }
    else if (enemy.name == "Человек-Муравей") {
        for (int i = 0; i < maxLines; i++) {
            if (i < 20) {
                cout << setw(40) << left << ANTMAN[i];
            }
            else {
                cout << setw(40) << left << " ";
            }

            if (i < count) {
                cout << setw(28) << left << lines[i];
            }
            else {
                cout << setw(28) << left << " ";
            }

            if (i < countPlayer) {
                cout << setw(20) << left << updatedLinesPlayer[i];
            }
            else {
                cout << setw(20) << left << " ";
            }

            cout << endl;
        }
    }
    else if (enemy.name == "БОСС") {
        for (int i = 0; i < maxLines; i++) {
            if (i < 20) {
                cout << setw(40) << left << BOSS[i];
            }
            else {
                cout << setw(40) << left << " ";
            }

            if (i < count) {
                cout << setw(28) << left << lines[i];
            }
            else {
                cout << setw(28) << left << " ";
            }

            if (i < countPlayer) {
                cout << setw(20) << left << updatedLinesPlayer[i];
            }
            else {
                cout << setw(20) << left << " ";
            }

            cout << endl;
        }
    }

    cout << "\nУправление: 1 - Атака | 2 - Защита | 3 - Лечение\n";
    if (player.hp <= 0) {
        cout << "Вы проиграли\n";
    }
    else if (enemy.hp <= 0) {
        cout << "Вы победили\n";
    }
}
bool isFight = true;
string action = "";
DWORD WINAPI Fight(LPVOID lpParam) {
    srand(time(0));
    Data* data = (Data*)lpParam;
    Player* p = data->pl;
    Enemy* e = data->en;
    e->armor *= p->difficultyMultyplier;
    e->hp *= p->difficultyMultyplier;
    e->power *= p->difficultyMultyplier;
    e->chanceOfCrit *= p->difficultyMultyplier;


    HANDLE eAttack = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"eAttack");
    HANDLE eDefend = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"eDefend");
    HANDLE eHeal = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"eHeal");



    while (isFight) {
        
        if (p->hp <= 0) {
            cout << "Вы проиграли, нажмите любую кнопку" << endl;
            isFight = false;
            break;
        }
        if (e->hp <= 0) {
            cout << "Вы выиграли, нажмите для выхода" << endl;
            isFight = false;
            break;
        }
        // event не занят - по кнопке бьем/защищаемся/лечимся
        if (WaitForSingleObject(eAttack, 0) != WAIT_TIMEOUT) {

            action += "Удар ";
            int chanceCritPlayer = rand() % 100;
            int dmgPlayer = p->power;
            int chanceCritEnemy = rand() % 100;
            int dmgEnemy = e->power;
            if (chanceCritPlayer < p->intellect + 1) {
                dmgPlayer *= 2;

                action += "\n(Крит у игрока)";
            }
            e->hp -= dmgPlayer;
            if (chanceCritEnemy < e->chanceOfCrit + 1) {
                dmgEnemy *= 2;
                action += "\n(Крит у врага)";
            }
            p->hp -= dmgEnemy;

            ResetEvent(eAttack);
        }

        if (WaitForSingleObject(eDefend, 0) != WAIT_TIMEOUT) {
            cout << "Защита\n";
            p->hp -= e->power * ((float)p->armor / 100.0f);
            e->hp -= p->power * 0.2;
            cout << "Хп игрока: " << p->hp << "\nХп врага: " << e->hp << endl;
            ResetEvent(eDefend);
        }

        if (WaitForSingleObject(eHeal, 0) != WAIT_TIMEOUT) {
            if (p->countOfHeal > 0) {
                cout << "Лечение\n";
                p->hp += 50;
                p->countOfHeal--;
                cout << "Хп игрока: " << p->hp << "\nХп врага: " << e->hp << endl;
            }
            else {
                cout << "У вас не осталось зелья исцеления" << endl;
            }
            ResetEvent(eHeal);
        }
    }



    CloseHandle(eAttack);
    CloseHandle(eDefend);
    CloseHandle(eHeal);
    return 0;

}

void PrintToData(Player* player) {
    std::ofstream data(URL_DATA); // Открытие файла // ЗАПИСЬ
    if (data.is_open()) { // запись

        data << player->hp << std::endl;
        data << player->power << std::endl;
        data << player->armor << std::endl;
        data << player->intellect << std::endl;
        data << player->countOfHeal << std::endl;
        data << player->difficultyMultyplier << std::endl;

        data.close();
    }
}

void ReadFromData(Player* player) {
    std::ifstream file(URL_DATA); // Открытие файла ЧТЕНИЕ

    if (file.is_open()) { // чтение

        file >> player->hp;
        file >> player->power;
        file >> player->armor;
        file >> player->intellect;
        file >> player->countOfHeal;
        file >> player->difficultyMultyplier;

        file.close();
    }
}


int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "rus");
    HANDLE eAttack = CreateEvent(NULL, TRUE, FALSE, L"eAttack");
    HANDLE eDefend = CreateEvent(NULL, TRUE, FALSE, L"eDefend");
    HANDLE eHeal = CreateEvent(NULL, TRUE, FALSE, L"eHeal");
    if (eAttack == NULL)
        return GetLastError();
    if (eDefend == NULL)
        return GetLastError();
    if (eHeal == NULL)
        return GetLastError();

    HANDLE hEndEvent;
    hEndEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"EndFightEvent");
    if (hEndEvent == NULL)
        return GetLastError();

    ResetEvent(hEndEvent);
    Data data;
    Enemy enemy;
    Player player;
    ifstream outBoss(URL_BOSS);
    if (outBoss.is_open()) {
        outBoss >> isBoss;
        outBoss.close();
    }

    string names[] = {
        "Слайм",
        "Человек-Муравей",
        "Циклоп",
    };

    if (isBoss) {
        enemy.name = "БОСС";
        enemy.armor *= 10;
        enemy.hp *= 10;
        enemy.power *= 10;
        enemy.chanceOfCrit *= 2;
    }
    else {
        int randomMonster = rand() % 3;
        enemy.name = names[randomMonster];
    }

    ReadFromData(&player);

    data.en = &enemy;
    data.pl = &player;

    DWORD IDThread;
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fight, &data, NULL, &IDThread);
    if (hThread == NULL)
        return GetLastError();

    string linesPlayer[7] = {
        "ИГРОК",
        "Характеристики игрока:",
        "HP: " + to_string(player.hp),
        "Сила: " + to_string(player.power),
        "Броня: " + to_string(player.armor),
        "Интеллект: " + to_string(player.intellect),
        "Зелья здоровья(шт): " + to_string(player.countOfHeal),
    };

    while (isFight) {
        UpdateInterface(enemy, player, linesPlayer);
        cout << action;
        action = "";
        switch (_getch()) {
        case 49:
            SetEvent(eAttack);
            break;
        case 50:
            SetEvent(eDefend);
            break;
        case 51:
            SetEvent(eHeal);
            break;
        default:break;
        }
    }
    bool isFinal = false;
    PrintToData(&player);
    if (isBoss && enemy.hp <= 0) {
        isFinal = true;
        ofstream fout(URL_FINAL);
        if (fout.is_open()) {
            fout << isFinal << endl;

            fout.close();
        }
    }
        
    
    if (player.hp <= 0 && !isFight) {

        while (true) {
            if (_getch() != NULL) {
                SetEvent(hEndEvent);
                TerminateProcess(GetCurrentProcess(), 1);
            }
        }

    }
    if (enemy.hp <= 0 && !isFight) {

        while (true) {
            switch (_getch()) {
            case 51:
                SetEvent(hEndEvent);
                TerminateProcess(GetCurrentProcess(), 1);
                break;
            default:
                cout << "Нажмите 3 для выхода";
                break;
            }
        }
    }


    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(eAttack);
    CloseHandle(eDefend);
    CloseHandle(eHeal);
    return 0;

}
