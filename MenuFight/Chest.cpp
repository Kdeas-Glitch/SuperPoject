
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
using namespace std;
struct Player
{
    int x = 5;
    int y = 5;
    int hp = 1000;
    int power = 7;
    int armor = 50; // резист от атаки при защите
    int intellect = 20; // шанс крита
    int countOfHeal = 7;
};
struct DataChest {
    // в будущем здесь будет структура врага(наврено)
    Player* pl;
};
DWORD WINAPI Chest(LPVOID lpParam) {
    DataChest* data = (DataChest*)lpParam;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    wchar_t figth_process[] = L"C:\\Users\\Leshu\\Desktop\\project\\SuperPoject\\x64\\Debug\\Fight.exe ";
    Player* p = data->pl;
    srand(time(NULL) + GetCurrentThreadId());
    cout << "Статы игрока:\n" << p->hp << "\n" << p->power << "\n" << p->intellect << endl;
    HANDLE chestOpen = OpenEvent(EVENT_ALL_ACCESS,FALSE,L"OpenChest");
    if (chestOpen == NULL) {
        return GetLastError();
    }
    while (true) {

        if (WaitForSingleObject(chestOpen, 0) != WAIT_TIMEOUT) {
            int chanceLoot = rand() % 100;
            int chanceMimik = rand() % 100;
            if (chanceMimik <= 10) {
                if (!CreateProcess(figth_process, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                    return GetLastError();
                }
                TerminateProcess(GetCurrentProcess(),1);
            }
            if (chanceLoot <= 25)
            {
                p->intellect++;
                cout << "Вы нашли книгу интеллекта" << p->intellect;
            }
            
            if (chanceLoot > 25 && chanceLoot <= 50) {

                p->power++;
                cout << "Вы нашли книгу силы" <<  p->power;
            }
            if (chanceLoot > 50 && chanceLoot <= 75) {
                p->countOfHeal++;
                cout << "Вы нашли хил" << p->countOfHeal;
            }
            if (chanceLoot > 75) {

                p->armor++;
                cout << "Вы нашли книгу шлем" << p->armor;
            }
            ResetEvent(chestOpen);
            
        }
       
    }
    CloseHandle(chestOpen);
    return 0;
}

int main()
{
    setlocale(LC_ALL, "rus");
    Player player;
    DataChest dataChest;
    dataChest.pl = &player;
    HANDLE chest = CreateEvent(NULL,TRUE,FALSE,L"OpenChest");
    if (chest == NULL) {
        return GetLastError();
    }
    DWORD IDThread;
    HANDLE hThread = CreateThread(NULL,NULL, (LPTHREAD_START_ROUTINE)Chest,&dataChest,0,&IDThread);
    if (hThread == NULL) {
        return GetLastError();
    }
    while (true) {
        switch (_getch()) {
        case 49: 
            SetEvent(chest);
            break;

        default:break;
        }
        
    }
    WaitForSingleObject(hThread,INFINITE);
    CloseHandle(hThread);
    return 0;
}
