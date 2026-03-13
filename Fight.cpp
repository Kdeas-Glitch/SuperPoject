#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <iomanip>
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
"               |          _____             |             ",
"               |        .'     '.           |             ",
"               |       /   _     \\         |             ",
"               |      |   (_)     |         |             ",
"               |      |           |         |             ",
"               |       \\         /         |             ",
"               |        '._   _.'           |             ",
"               |           '-'              |             ",
"               |      _____________         |             ",
"               |    / | | | | | | | \\      |             ",
"               |   |  |_|_|_|_|_|_|  |      |             ",
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
"                /    .' '.       .' '.    \\                       ",
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
struct Player
{
    int x=5;
    int y=5;
    int hp= 1000;
    int power = 23;
    int armor = 51; // резист от атаки при защите
    int intellect= 22; // шанс крита
    int countOfHeal = 7;
    int difficultyMultyplier = 2;
}; 
struct Enemy
{
    string name = "Человек-Муравей";
    int x=5;
    int y=5;
    int hp = 1000;
    int power = 28;
    int armor = 7;
    int chanceOfCrit = 10;
};
struct Data { // структура для передачи в поток Fight
    Enemy* en;
    Player* pl;
};
string lines[5] = {
"",
"",
"",
"",
""
};
void UpdateInterface(Enemy& enemy, Player& player, string linesPlayer[]) {
    system("cls");

    
    string lines[5] = {
        "ВРАГ: " + enemy.name,
        "Характеристики врага:",
        "HP: " + to_string(enemy.hp),
        "Сила: " + to_string(enemy.power),
        "Шанс крита: " + to_string(enemy.chanceOfCrit)
    };

   
    string updatedLinesPlayer[5] = {
        "ИГРОК",
        "Характеристики игрока:",
        "HP: " + to_string(player.hp),
        "Сила: " + to_string(player.power),
        "Интеллект: " + to_string(player.intellect)
    };

    int count = 5;
    int countPlayer = 5;
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

    cout << "\nУправление: 1 - Атака | 2 - Защита | 3 - Лечение\n";
    if (player.hp <= 0) {
        cout << "Вы проиграли\n";
    }
    else if (enemy.hp <= 0) {
        cout << "Вы победили\n";
    }
}
bool isFight = true;
DWORD WINAPI Fight(LPVOID lpParam) {
    srand(time(0));
    Data* data = (Data*)lpParam;
    Player* p = data->pl;
    Enemy* e = data->en;
    e->armor *= p->difficultyMultyplier;
    e->hp *= p->difficultyMultyplier;
    e->power *= p->difficultyMultyplier;
    e->chanceOfCrit *= p->difficultyMultyplier;
    
    
    HANDLE eAttack = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"eAttack");
    HANDLE eDefend = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"eDefend");
    HANDLE eHeal = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"eHeal");
    
    

    while (isFight) {
       
        if(p->hp <= 0){
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
            Sleep(200);
            cout << "Удар\n";
            int chanceCritPlayer = rand() % 100;
            int dmgPlayer = p->power;
            int chanceCritEnemy = rand() % 100;
            int dmgEnemy = e->power;
            if (chanceCritPlayer < p->intellect+1) {
                dmgPlayer *=2;
                cout << "Крит у игрока\n";
            }
            e->hp -= dmgPlayer;
            if (chanceCritEnemy < e->chanceOfCrit + 1) {
                dmgEnemy *= 2;
                cout << "Крит у врага\n";
            }
            p->hp -=dmgEnemy;

            ResetEvent(eAttack);
        }
       
        if (WaitForSingleObject(eDefend, 0) != WAIT_TIMEOUT) { 
            cout << "Защита\n";
            p->hp -= e->power * (p->armor / 100);
            e->hp -= p->power * 0.2;
            cout << "Хп игрока: " << p->hp <<"\nХп врага: " << e->hp << endl;
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

int main()
{
    setlocale(LC_ALL, "rus");
    HANDLE eAttack = CreateEvent(NULL,TRUE,FALSE,L"eAttack");
    HANDLE eDefend = CreateEvent(NULL, TRUE, FALSE, L"eDefend");
    HANDLE eHeal = CreateEvent(NULL, TRUE, FALSE, L"eHeal");
    if (eAttack == NULL) 
        return GetLastError();
    if (eDefend == NULL) 
        return GetLastError();
    if (eHeal == NULL) 
        return GetLastError();
    
    Data data;
    Enemy enemy;
    Player player;
    
    data.en = &enemy;
    data.pl = &player;

    DWORD IDThread;
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fight, &data, NULL, &IDThread);
    if (hThread == NULL) 
        return GetLastError();

    string linesPlayer[5] = {
        "ИГРОК",
        "Характеристики врага:",
        "HP: " + to_string(player.hp),
        "Сила: " + to_string(player.power),
        "Интеллект: " + to_string(player.intellect)
    };
    
    while (isFight) {
        UpdateInterface(enemy,player,linesPlayer);
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
    
    ofstream fout("data.txt");
    if (fout.is_open()) {
        fout << player.hp << endl;
        fout << player.power << endl;
        fout << player.armor << endl;
        fout << player.intellect << endl;
        fout << player.countOfHeal << endl;
        fout << player.difficultyMultyplier << endl;
        fout.close();
        std::cout << "Файл успешно записан." << std::endl;
    }

    if (player.hp <= 0 && !isFight) {
        
        while (true) {
            if (_getch() != NULL) {
                TerminateProcess(GetCurrentProcess(), 1);
            }
        }

    }
    if (enemy.hp <= 0 && !isFight) {
        
        while (true) {
            switch (_getch()) {
            case 51:
                TerminateProcess(GetCurrentProcess(),1);
                break;
            default:
                cout << "Нажмите 3 для выхода";
                break;
            }  
        }
    }

    
    WaitForSingleObject(hThread,INFINITE);
    CloseHandle(hThread);
    CloseHandle(eAttack);
    CloseHandle(eDefend);
    CloseHandle(eHeal);
    return 0;
    
}
