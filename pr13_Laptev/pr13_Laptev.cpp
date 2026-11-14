#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>

using namespace std;

//структура участника
struct Participant
{
    int id;
    PROCESS_INFORMATION pi;
    DWORD priorityClass;
    bool isFinished;
    DWORD startTime;
    DWORD endTime;
    double duration;
    int place;
};

bool isPrime(int n)
{
    if (n < 2)
    {
        return false;
    }

    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }

    return true;
}

//названия приоритета
string getPriorityName(DWORD p)
{
    if (p == IDLE_PRIORITY_CLASS) return "Низкий (IDLE)";
    if (p == BELOW_NORMAL_PRIORITY_CLASS) return "Ниже среднего (BELOW_NORMAL)";
    if (p == NORMAL_PRIORITY_CLASS) return "Обычный (NORMAL)";
    if (p == ABOVE_NORMAL_PRIORITY_CLASS) return "Выше среднего (ABOVE_NORMAL)";
    if (p == HIGH_PRIORITY_CLASS) return "Высокий (HIGH)";
    return "Неизвестно";
}

DWORD getPriorityValue(int choice)
{
    if (choice == 1) return IDLE_PRIORITY_CLASS;
    if (choice == 2) return BELOW_NORMAL_PRIORITY_CLASS;
    if (choice == 3) return NORMAL_PRIORITY_CLASS;
    if (choice == 4) return ABOVE_NORMAL_PRIORITY_CLASS;
    if (choice == 5) return HIGH_PRIORITY_CLASS;
    return NORMAL_PRIORITY_CLASS;
}

void showStatus(Participant participants[], int count)
{
    cout << "\nТекущее состояние гонки:" << endl;
    for (int i = 0; i < count; i++)
    {
        cout << "Участник " << participants[i].id << ", ID: " << participants[i].pi.dwProcessId << ", Приоритет: " << getPriorityName(participants[i].priorityClass) << ", Состояние: ";

        if (participants[i].isFinished)
        {
            cout << "Завершил (место: " << participants[i].place << ", время: " << participants[i].duration << " сек)" << endl;
        }
        else
        {
            cout << "Работает" << endl;
        }
    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    if (argc >= 3)
    {
        int start = stoi(argv[1]);
        int end = stoi(argv[2]);

        int count = 0;
        for (int i = start; i <= end; i++)
        {
            if (isPrime(i))
            {
                count++;
            }
        }

        cout << "Найдено простых чисел: " << count << endl;
        return 0;
    }

    string pathProc = "\\\\Mac\\Home\\Desktop\\Программирование\\СП_Гариев\\pr13_Laptev\\ARM64\\Debug\\worker.exe";

    int countParticipants = 0;
    cout << "Введите количество участников (от 2 до 5): ";
    cin >> countParticipants;

    if (countParticipants < 2 || countParticipants > 5)
    {
        cout << "Некорректное число участников. Должно быть от 2 до 5" << endl;
        return 0;
    }

    int startVal = 1;
    int endVal = 15000000;
    cout << "Введите конец диапазона: ";
    cin >> endVal;



    Participant participants[5];

    cout << "\n1. Установить всем одинаковый приоритет" << endl;
    cout << "2. Настроить разные приоритеты каждому" << endl;
    cout << "Выбор: ";
    int modeChoice = 0;
    cin >> modeChoice;

    DWORD defaultPriority = NORMAL_PRIORITY_CLASS;
    if (modeChoice == 1)
    {
        cout << "\nВыберите общий приоритет:" << endl;
        cout << "1. IDLE (Низкий)" << endl;
        cout << "2. BELOW_NORMAL (Ниже среднего)" << endl;
        cout << "3. NORMAL (Обычный)" << endl;
        cout << "4. ABOVE_NORMAL (Выше среднего)" << endl;
        cout << "5. HIGH (Высокий)" << endl;
        cout << "Выбор: ";
        int pr = 3;
        cin >> pr;
        defaultPriority = getPriorityValue(pr);
    }

    //создание
    for (int i = 0; i < countParticipants; i++)
    {
        participants[i].id = i + 1;
        participants[i].isFinished = false;
        participants[i].duration = 0.0;
        participants[i].place = 0;

        if (modeChoice == 2)
        {
            cout << "\nПриоритет для участника " << i + 1 << ":" << endl;
            cout << "1. IDLE, 2. BELOW, 3. NORMAL, 4. ABOVE, 5. HIGH" << endl;
            cout << "Выбор: ";
            int pr = 3;
            cin >> pr;
            participants[i].priorityClass = getPriorityValue(pr);
        }
        else
        {
            participants[i].priorityClass = defaultPriority;
        }

        STARTUPINFOA si;
        PROCESS_INFORMATION piApp;
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        si.cb = sizeof(STARTUPINFOA);
        ZeroMemory(&piApp, sizeof(PROCESS_INFORMATION));

        string command = "\"" + pathProc + "\" " + to_string(startVal) + " " + to_string(endVal);
        char cmd[512];
        strcpy_s(cmd, sizeof(cmd), command.c_str());

        if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp))
        {
            cout << "Не удалось создать процесс " << i + 1 << endl;
            return 0;
        }

        participants[i].pi = piApp;

        if (!SetPriorityClass(piApp.hProcess, participants[i].priorityClass))
        {
            cout << "Ошибка установки приоритета для процесса " << i + 1 << endl;
        }

        cout << "Создан участник " << i + 1 << ", ID: " << piApp.dwProcessId << ", приоритет: " << getPriorityName(participants[i].priorityClass) << endl;
    }

    cout << "\nНажмите для старта" << endl;
    _getch();

    //запуск всех
    DWORD startTime = GetTickCount();
    for (int i = 0; i < countParticipants; i++)
    {
        participants[i].startTime = startTime;
        ResumeThread(participants[i].pi.hThread);
    }

    cout << "\nГонка началась!" << endl;
    cout << "Управление во время гонки:" << endl;
    cout << "Нажмите клавишу с номером участника (1-" << countParticipants << ") для изменения приоритета" << endl;
    cout << "Нажмите пробел для просмотра статуса" << endl;

    int finishedCount = 0;

    //цикл гонки
    while (finishedCount < countParticipants)
    {
        for (int i = 0; i < countParticipants; i++)
        {
            if (!participants[i].isFinished)
            {
                DWORD exitCode = 0;
                if (GetExitCodeProcess(participants[i].pi.hProcess, &exitCode))
                {
                    if (exitCode != STILL_ACTIVE)
                    {
                        participants[i].isFinished = true;
                        participants[i].endTime = GetTickCount();
                        participants[i].duration = (participants[i].endTime - participants[i].startTime) / 1000.0;
                        finishedCount++;
                        participants[i].place = finishedCount;

                        cout << "\nУчастник " << participants[i].id << " (ID: " << participants[i].pi.dwProcessId << ") финишировал! Место: " << participants[i].place << ", время: " << participants[i].duration << " сек" << endl;
                    }
                }
            }
        }

        if (_kbhit())
        {
            char key = _getch();

            //изменение приоритета
            if (key >= '1' && key < '1' + countParticipants)
            {
                int idx = key - '1';

                if (!participants[idx].isFinished)
                {
                    cout << "\nСмена приоритета участника " << idx + 1 << ":" << endl;
                    cout << "1. IDLE, 2. BELOW, 3. NORMAL, 4. ABOVE, 5. HIGH" << endl;
                    cout << "Новый приоритет (нажмите 1-5): ";

                    char prKey = _getch();
                    cout << prKey << endl;

                    int newChoice = prKey - '0';
                    DWORD newPr = getPriorityValue(newChoice);
                    if (SetPriorityClass(participants[idx].pi.hProcess, newPr))
                    {
                        participants[idx].priorityClass = newPr;
                        cout << "Приоритет участника " << idx + 1 << " изменен на: " << getPriorityName(newPr) << endl;
                    }
                    else
                    {
                        cout << "Ошибка изменения приоритета" << endl;
                    }
                }
                else
                {
                    cout << "\nУчастник " << idx + 1 << " уже финишировал" << endl;
                }
            }
            else if (key == ' ')
            {
                showStatus(participants, countParticipants);
            }
        }

        Sleep(50);
    }

    //сортировка
    for (int i = 0; i < countParticipants - 1; i++)
    {
        for (int j = 0; j < countParticipants - 1 - i; j++)
        {
            if (participants[j].place > participants[j + 1].place)
            {
                Participant temp = participants[j];
                participants[j] = participants[j + 1];
                participants[j + 1] = temp;
            }
        }
    }

    cout << "\nИтоговая таблица результатов гонки:" << endl;
    for (int i = 0; i < countParticipants; i++)
    {
        cout << participants[i].place << " место: участник " << participants[i].id << ", ID: " << participants[i].pi.dwProcessId << ", приоритет: " << getPriorityName(participants[i].priorityClass) << ", время: " << participants[i].duration << " сек" << endl;
    }

    for (int i = 0; i < countParticipants; i++)
    {
        CloseHandle(participants[i].pi.hProcess);
        CloseHandle(participants[i].pi.hThread);
    }

    _getch();
    return 0;
}