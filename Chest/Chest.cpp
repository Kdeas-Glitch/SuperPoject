
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <string>

#define URL_DATA "data.txt"
#define URL_FIGHT_PROCESS L"Fight.exe "
#define CHANCE_FOR_MIMIC 50


using namespace std;
struct Player
{
    int x;
    int y;
    int hp;
    int power;
    int armor; // резист от атаки при защите
    int intellect; // шанс крита
    int countOfHeal;
    int difficultyMultyplier;
};

struct DataChest {
    // в будущем здесь будет структура врага(наврено)
    Player* pl;
};



bool isLooted = false;
DWORD WINAPI Chest(LPVOID lpParam) {
    DataChest* data = (DataChest*)lpParam;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    wchar_t figth_process[] = URL_FIGHT_PROCESS;
    Player* p = data->pl;
    srand(time(NULL) + GetCurrentThreadId());

    string linesPlayer[7] = {
         "ИГРОК",
         "Характеристики игрока:",
         "HP: " + to_string(p->hp),
         "Сила: " + to_string(p->power),
         "Броня: " + to_string(p->armor),
         "Интеллект: " + to_string(p->intellect),
         "Зелья здоровья(шт): " + to_string(p->countOfHeal),
    };

    for (int i = 0; i < 7; i++) {
        cout << linesPlayer[i] << endl;
    }
    cout << "1 - Открыть | 2 - Уйти\n";
    HANDLE chestOpen = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"OpenChest");
    if (chestOpen == NULL) {
        return GetLastError();
    }
        
    
    while (!isLooted) {

        if (WaitForSingleObject(chestOpen, 0) != WAIT_TIMEOUT) {
            int chanceLoot = rand() % 100;
            int chanceMimik = rand() % 100;
            if (chanceMimik <= CHANCE_FOR_MIMIC) {

                if (!CreateProcess(figth_process, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                    return GetLastError();
                }
                TerminateProcess(GetCurrentProcess(), 1);
            }
            if (chanceLoot <= 25)
            {
                p->intellect++;
                cout << "Вы нашли книгу интеллекта +1";
                isLooted = true;
                
            }

            if (chanceLoot > 25 && chanceLoot <= 50) {
                
                p->power++;
                cout << "Вы нашли книгу силы +1";
                isLooted = true;
                
            }
            if (chanceLoot > 50 && chanceLoot <= 75) {
                
                p->countOfHeal++;
                cout << "Вы нашли зелье здоровья +1";
                isLooted = true;
                
            }
            if (chanceLoot > 75) {

                p->armor++;
                cout << "Вы нашли броню, броня +1";
                isLooted = true;
               
            }
           
            ofstream fout(URL_DATA);
            if (fout.is_open()) {
                fout << p->hp << endl;
                fout << p->power << endl;
                fout << p->armor << endl;
                fout << p->intellect << endl;
                fout << p->countOfHeal << endl;
                fout << p->difficultyMultyplier << endl;
               
                fout.close();
                break;
            }
            ResetEvent(chestOpen);

        }

    }
    cout << "\nНажмите чтобы выйти:";
    CloseHandle(chestOpen);
    return 0;
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
    
    setlocale(LC_ALL, "rus");
    Player player;
    ReadFromData(&player);
    DataChest dataChest;
    dataChest.pl = &player;
    HANDLE chest = CreateEvent(NULL, TRUE, FALSE, L"OpenChest");
    if (chest == NULL) {
        return GetLastError();
    }
    DWORD IDThread;
    HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Chest, &dataChest, 0, &IDThread);
    if (hThread == NULL) {
        return GetLastError();
    }
    HANDLE hEndEvent;
    hEndEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE, (LPCWSTR)"EndChestEvent");
    if (hEndEvent == NULL)
        return GetLastError();

    while (!isLooted) {
        switch (_getch()) {
        case 49:
            SetEvent(chest);
            SetEvent(hEndEvent);
            break;
        case 50:
            SetEvent(hEndEvent);
            TerminateProcess(GetCurrentProcess(), 0);
            break;
        default:break;
        }

    }

    while (isLooted) {
        if (_getch() != NULL) {
            SetEvent(hEndEvent);
            TerminateProcess(GetCurrentProcess(), 1);
        }
    }
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    return 0;
}
