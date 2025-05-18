#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <algorithm>
#include <gdiplus.h>
#include <fstream>

#include "genetic_cnf.hpp"
#include "simulated_annealing_cnf.hpp"

#define UNICODE
#define _UNICODE

#define WIN32_LEAN_AND_MEAN

#define ID_BUTTON_GA 1001
#define ID_BUTTON_SA 1002

#define ID_GENERATE_CNF_BUTTON 1003
#define ID_GENERATE_CANDIDATES_BUTTON 1004

#define ID_FUNCTION_SIZE_EDIT 1005
#define ID_ITERATIONS_EDIT 1006
#define ID_POPULATION_EDIT 1007
#define ID_CROSSOVERS_EDIT 1008
#define ID_MUTATIONS_EDIT 1009
#define ID_GENES_EDIT 1010
#define ID_SELECTION_FUNCTION_EDIT 1011

#define ID_RUN_ALGORITHM_BUTTON 1012

#define ID_GRAPH_WINDOW 1020 

#define ID_INITIAL_TEMP_EDIT 1025
#define ID_MIN_TEMP_EDIT 1026
#define ID_COOLING_RATE_EDIT 1027
#define ID_COOLING_TYPE_COMBO 1028

#define ID_GENERATE_INITIAL_SOLUTION_BUTTON 1029
#define ID_LOAD_CANDIDATE_BUTTON 1030

static std::vector<double> g_ga_best_qualities;  
static std::vector<double> g_sa_best_qualities;  

static std::wstring g_graph_title = L""; 


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK GAWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SAWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

double GetDlgItemDouble(HWND hDlg, int nIDDlgItem);

void RegisterGAWindowClass(HINSTANCE hInstance);
void RegisterSAWindowClass(HINSTANCE hInstance);
void RegisterGraphWindowClass(HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    _wsetlocale(LC_ALL, L"Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    _CRT_UNUSED(hPrevInstance);
    _CRT_UNUSED(lpCmdLine);

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) }; 
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("MainWindowClass");

    if (!RegisterClassEx(&wc))
    {
        MessageBoxW(NULL, _T("Ошибка регистрации класса окна!"), _T("Ошибка"), MB_ICONERROR);
        return 1;
    }

    RegisterGAWindowClass(hInstance);
    RegisterSAWindowClass(hInstance);
    RegisterGraphWindowClass(hInstance);

    HWND hWnd = CreateWindowEx(
        0,
        _T("MainWindowClass"),
        _T("Выполнимость КНФ"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        DWORD error = GetLastError();
        MessageBox(nullptr, (L"Ошибка: " + std::to_wstring(error)).c_str(), L"Ошибка", MB_OK);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void RegisterGAWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GAWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("GAWindowClass");

    RegisterClassEx(&wc);
}

void RegisterSAWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = SAWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("SAWindowClass");

    RegisterClassEx(&wc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindowW(
            _T("BUTTON"), _T("Генетический алгоритм"),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            100, 50, 200, 50, hWnd, (HMENU)ID_BUTTON_GA, NULL, NULL);

        CreateWindowW(
            _T("BUTTON"), _T("Алгоритм имитации отжига"),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            100, 120, 200, 50, hWnd, (HMENU)ID_BUTTON_SA, NULL, NULL);

        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_BUTTON_GA:
        {
            HWND hGAWnd = CreateWindowEx(
                0,
                _T("GAWindowClass"),
                _T("Генетический алгоритм"),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                hWnd, NULL, NULL, NULL);

            if (hGAWnd)
            {
                ShowWindow(hGAWnd, SW_SHOW);
                UpdateWindow(hGAWnd);
            }
            break;
        }
        case ID_BUTTON_SA:
        {
            HWND hSAWnd = CreateWindowEx(
                0,
                _T("SAWindowClass"),
                _T("Алгоритм имитации отжига"),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                hWnd, NULL, NULL, NULL);

            if (hSAWnd)
            {
                ShowWindow(hSAWnd, SW_SHOW);
                UpdateWindow(hSAWnd);
            }
            break;
        }
        }
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK GAWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    static HWND hSelectionCombo;
    static HFONT hFont;

    static CNF *p_cnf = nullptr;
    static Candidates *pCandidates = nullptr;
    static GeneticAlgorithm *pGeneticAlgorithm = nullptr;

    switch (message)
    {
    case WM_CREATE:
    {
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE, L"Arial");

        CreateWindowW(L"STATIC", L"Настройки генетического алгоритма для задачи выполнимости КНФ",
                      WS_VISIBLE | WS_CHILD,
                      20, 20, 500, 25, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"STATIC", L"Размер функции (переменных):",
                      WS_VISIBLE | WS_CHILD,
                      20, 60, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"10",
                                          WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                          220, 60, 100, 20, hWnd, (HMENU)ID_FUNCTION_SIZE_EDIT, NULL, NULL);

        CreateWindowW(L"STATIC", L"Количество итераций:",
                      WS_VISIBLE | WS_CHILD,
                      20, 90, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"1000",
                                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                        220, 90, 100, 20, hWnd, (HMENU)ID_ITERATIONS_EDIT, NULL, NULL);
        CreateWindowW(L"STATIC", L"Размер популяции:",
                      WS_VISIBLE | WS_CHILD,
                      20, 120, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"100",
                                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                        220, 120, 100, 20, hWnd, (HMENU)ID_POPULATION_EDIT, NULL, NULL);
        CreateWindowW(L"STATIC", L"Количество скрещиваний:",
                      WS_VISIBLE | WS_CHILD,
                      20, 150, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"50",
                                        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                        220, 150, 100, 20, hWnd, (HMENU)ID_CROSSOVERS_EDIT, NULL, NULL);
        CreateWindowW(L"STATIC", L"Количество мутаций:",
                      WS_VISIBLE | WS_CHILD,
                      20, 180, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"20",
                                       WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                       220, 180, 100, 20, hWnd, (HMENU)ID_MUTATIONS_EDIT, NULL, NULL);
        CreateWindowW(L"STATIC", L"Количество генов при мутации:",
                      WS_VISIBLE | WS_CHILD,
                      20, 210, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"3",
                                   WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                   220, 210, 100, 20, hWnd, (HMENU)ID_GENES_EDIT, NULL, NULL);
        CreateWindowW(L"STATIC", L"Функция выбора:",
                      WS_VISIBLE | WS_CHILD,
                      20, 240, 200, 20, hWnd, NULL, NULL, NULL);
        hSelectionCombo = CreateWindowW(L"COMBOBOX", NULL,
                                        WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
                                        220, 240, 200, 100, hWnd, (HMENU)ID_SELECTION_FUNCTION_EDIT, NULL, NULL);

        SendMessageW(hSelectionCombo, CB_ADDSTRING, 0, (LPARAM)L"Случайная");
        SendMessageW(hSelectionCombo, CB_ADDSTRING, 0, (LPARAM)L"Линейная");
        SendMessageW(hSelectionCombo, CB_ADDSTRING, 0, (LPARAM)L"Экспоненциальная");
        SendMessageW(hSelectionCombo, CB_SETCURSEL, 0, 0);

        CreateWindowW(L"BUTTON", L"Сгенерировать КНФ",
                                           WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                           20, 280, 180, 30, hWnd, (HMENU)ID_GENERATE_CNF_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Сгенерировать кандидатов",
                                                  WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                                  220, 280, 180, 30, hWnd, (HMENU)ID_GENERATE_CANDIDATES_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Выполнить алгоритм",
                      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                      20, 320, 380, 30, hWnd, (HMENU)ID_RUN_ALGORITHM_BUTTON, NULL, NULL);

        EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL
                         {
                SendMessage(hwnd, WM_SETFONT, lParam, TRUE);
                return TRUE; }, (LPARAM)hFont);

        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == ID_GENERATE_CNF_BUTTON)
        {
            std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
            p_cnf = new CNF(function_length);

            MessageBoxW(hWnd, L"КНФ сгенерирована", L"Информация", MB_OK);
        }
        else if (wmId == ID_GENERATE_CANDIDATES_BUTTON)
        {
            std::size_t population = GetDlgItemInt(hWnd, ID_POPULATION_EDIT, NULL, FALSE);
            std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
            pCandidates = new Candidates(population, function_length);

            MessageBoxW(hWnd, L"Кандидаты сгенерированы", L"Информация", MB_OK);
        }
        else if (wmId == ID_RUN_ALGORITHM_BUTTON)
        {
            g_graph_title = L"Генетический алгоритм";
            if (p_cnf == nullptr || pCandidates == nullptr)
            {
                MessageBoxW(hWnd, L"Сначала сгенерируйте КНФ и кандидатов!", L"Ошибка", MB_ICONERROR);
                break;
            }

            std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
            std::size_t population = GetDlgItemInt(hWnd, ID_POPULATION_EDIT, NULL, FALSE);
            std::size_t iterations = GetDlgItemInt(hWnd, ID_ITERATIONS_EDIT, NULL, FALSE);
            std::size_t hybridizations = GetDlgItemInt(hWnd, ID_CROSSOVERS_EDIT, NULL, FALSE);
            std::size_t mutations = GetDlgItemInt(hWnd, ID_MUTATIONS_EDIT, NULL, FALSE);
            std::size_t amount_gens_mutations = GetDlgItemInt(hWnd, ID_GENES_EDIT, NULL, FALSE);

            if (hybridizations > population)
            {
                MessageBoxW(
                    hWnd, 
                    L"Количество скрещиваний не может быть больше размера популяции", 
                    L"Ошибка",
                    MB_ICONERROR
                );
                break;
            }

            if (mutations > population)
            {
                MessageBoxW(
                    hWnd, 
                    L"Количество мутаций не может быть больше размера популяции", 
                    L"Ошибка",
                    MB_ICONERROR
                );
                break;
            }

            if (amount_gens_mutations > function_length)
            {
                MessageBoxW(
                    hWnd, 
                    L"Количество генов при мутации не может быть больше размера функций", 
                    L"Ошибка",
                    MB_ICONERROR
                );
                break;
            }

            int selectionIndex = SendMessage(hSelectionCombo, CB_GETCURSEL, 0, 0);
            selection_function sf = selection_function::RANDOM;
            if (selectionIndex == 1)
                sf = selection_function::LINEAR;
            else if (selectionIndex == 2)
                sf = selection_function::EXPONENTIAL;

            pGeneticAlgorithm = new GeneticAlgorithm(
                *p_cnf,
                *pCandidates,
                sf);

            auto result = pGeneticAlgorithm->execute(
                iterations,
                population,
                hybridizations,
                mutations,
                amount_gens_mutations);

            std::size_t iteration = std::get<0>(result);
            std::vector<double> best_qualities = std::get<1>(result);
            std::string answer = std::get<2>(result);

            std::wstring wAnswer(answer.begin(), answer.end());

            std::wstring message = L"Алгоритм завершен!\n";
            message += L"Итерация: " + std::to_wstring(iteration) + L"\n";
            message += L"Ответ: " + wAnswer;

            MessageBoxW(hWnd, message.c_str(), L"Результат выполнения", MB_OK);
            InvalidateRect(hWnd, NULL, TRUE);

            g_ga_best_qualities = best_qualities;

            std::cout << g_ga_best_qualities.size() << '\n';

            // Создаем новое окно для графика
            HWND hGraphWnd = CreateWindowEx(
                0,
                _T("GraphWindowClass"),
                _T("График функции качества"),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                hWnd, NULL, NULL, NULL);

            if (hGraphWnd)
            {
                ShowWindow(hGraphWnd, SW_SHOW);
                UpdateWindow(hGraphWnd);
            }
        }
        break;
    }
    case WM_DESTROY:
    {
        if (pGeneticAlgorithm)
        {
            delete pGeneticAlgorithm;
            pGeneticAlgorithm = nullptr;
        }

        if (p_cnf)
        {
            delete p_cnf;
            p_cnf = nullptr;
        }

        if (pCandidates)
        {
            delete pCandidates;
            pCandidates = nullptr;
        }

        if (hFont)
            DeleteObject(hFont);

        DestroyWindow(hWnd);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK SAWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hCoolingCombo;
    static HFONT hFont;
    
    static CNF* p_cnf = nullptr;
    static Candidate* pInitialSolution = nullptr;
    static SaAlgorithm* pSAAlgorithm = nullptr;

    switch (message)
    {
    case WM_CREATE:
    {
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_DONTCARE, L"Arial");

        // Заголовок окна
        CreateWindowW(L"STATIC", L"Настройки алгоритма имитации отжига для задачи выполнимости КНФ",
                     WS_VISIBLE | WS_CHILD,
                     20, 20, 500, 25, hWnd, NULL, NULL, NULL);

        // Поле для размера функции (количество переменных)
        CreateWindowW(L"STATIC", L"Размер функции (переменных):",
                     WS_VISIBLE | WS_CHILD,
                     20, 60, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"10",
                     WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                     220, 60, 100, 20, hWnd, (HMENU)ID_FUNCTION_SIZE_EDIT, NULL, NULL);

        // Начальная температура
        CreateWindowW(L"STATIC", L"Начальная температура:",
                     WS_VISIBLE | WS_CHILD,
                     20, 90, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"100.0",
                     WS_VISIBLE | WS_CHILD | WS_BORDER,
                     220, 90, 100, 20, hWnd, (HMENU)ID_INITIAL_TEMP_EDIT, NULL, NULL);

        // Минимальная температура
        CreateWindowW(L"STATIC", L"Минимальная температура:",
                     WS_VISIBLE | WS_CHILD,
                     20, 120, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"0.1",
                     WS_VISIBLE | WS_CHILD | WS_BORDER,
                     220, 120, 100, 20, hWnd, (HMENU)ID_MIN_TEMP_EDIT, NULL, NULL);

        // Скорость охлаждения
        CreateWindowW(L"STATIC", L"Скорость охлаждения:",
                     WS_VISIBLE | WS_CHILD,
                     20, 150, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"0.95",
                     WS_VISIBLE | WS_CHILD | WS_BORDER,
                     220, 150, 100, 20, hWnd, (HMENU)ID_COOLING_RATE_EDIT, NULL, NULL);

        // Количество итераций
        CreateWindowW(L"STATIC", L"Количество итераций:",
                     WS_VISIBLE | WS_CHILD,
                     20, 180, 200, 20, hWnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", L"10000",
                     WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                     220, 180, 100, 20, hWnd, (HMENU)ID_ITERATIONS_EDIT, NULL, NULL);

        // Тип охлаждения
        CreateWindowW(L"STATIC", L"Тип охлаждения:",
                     WS_VISIBLE | WS_CHILD,
                     20, 210, 200, 20, hWnd, NULL, NULL, NULL);
        hCoolingCombo = CreateWindowW(L"COMBOBOX", NULL,
                                    WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
                                    220, 210, 200, 100, hWnd, (HMENU)ID_COOLING_TYPE_COMBO, NULL, NULL);

        SendMessageW(hCoolingCombo, CB_ADDSTRING, 0, (LPARAM)L"Линейное");
        SendMessageW(hCoolingCombo, CB_ADDSTRING, 0, (LPARAM)L"Экспоненциальное");
        SendMessageW(hCoolingCombo, CB_SETCURSEL, 1, 0); 

        CreateWindowW(L"BUTTON", L"Сгенерировать КНФ",
                     WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     20, 250, 180, 30, hWnd, (HMENU)ID_GENERATE_CNF_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Сгенерировать начальное решение",
                     WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     220, 250, 180, 30, hWnd, (HMENU)ID_GENERATE_INITIAL_SOLUTION_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Загрузить решение из файла",
                     WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     20, 290, 180, 30, hWnd, (HMENU)ID_LOAD_CANDIDATE_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Выполнить алгоритм",
                     WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                     220, 290, 180, 30, hWnd, (HMENU)ID_RUN_ALGORITHM_BUTTON, NULL, NULL);

        // Установка шрифта для всех элементов
        EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
            SendMessage(hwnd, WM_SETFONT, lParam, TRUE);
            return TRUE;
        }, (LPARAM)hFont);

        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == ID_GENERATE_CNF_BUTTON)
        {
            std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
            if (p_cnf) delete p_cnf;
            p_cnf = new CNF(function_length);
            MessageBoxW(hWnd, L"КНФ сгенерирована", L"Информация", MB_OK);
        }
        else if (wmId == ID_GENERATE_INITIAL_SOLUTION_BUTTON)
        {
            std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
            if (pInitialSolution) delete pInitialSolution;
            pInitialSolution = new Candidate(function_length);
            MessageBoxW(hWnd, L"Начальное решение сгенерировано", L"Информация", MB_OK);
        }
        else if (wmId == ID_LOAD_CANDIDATE_BUTTON)
        {
            // Диалог открытия файла
            OPENFILENAMEW ofn;
            wchar_t szFileName[MAX_PATH] = L"";

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Текстовые файлы (*.txt)\0*.txt\0Все файлы (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = L"txt";

            if (GetOpenFileNameW(&ofn))
            {
                std::ifstream file(szFileName);
                if (file.is_open())
                {
                    std::string candidate_str;
                    file >> candidate_str;
                    file.close();

                    std::size_t function_length = GetDlgItemInt(hWnd, ID_FUNCTION_SIZE_EDIT, NULL, FALSE);
                    if (candidate_str.length() != function_length)
                    {
                        MessageBoxW(hWnd, 
                                  L"Длина кандидата не соответствует размеру функции!", 
                                  L"Ошибка", 
                                  MB_ICONERROR);
                    }
                    else
                    {
                        if (pInitialSolution) delete pInitialSolution;
                        pInitialSolution = new Candidate(function_length);
                        pInitialSolution->set_function(candidate_str);
                        MessageBoxW(hWnd, L"Кандидат успешно загружен из файла", L"Информация", MB_OK);
                    }
                }
                else
                {
                    MessageBoxW(hWnd, L"Не удалось открыть файл", L"Ошибка", MB_ICONERROR);
                }
            }
        }
        else if (wmId == ID_RUN_ALGORITHM_BUTTON)
        {
            if (!p_cnf || !pInitialSolution)
            {
                MessageBoxW(hWnd, L"Сначала сгенерируйте КНФ и начальное решение!", L"Ошибка", MB_ICONERROR);
                break;
            }

            double initial_temp = GetDlgItemDouble(hWnd, ID_INITIAL_TEMP_EDIT);
            double min_temp = GetDlgItemDouble(hWnd, ID_MIN_TEMP_EDIT);
            double cooling_rate = GetDlgItemDouble(hWnd, ID_COOLING_RATE_EDIT);
            std::size_t iterations = GetDlgItemInt(hWnd, ID_ITERATIONS_EDIT, NULL, FALSE);
            
            int coolingIndex = SendMessage(hCoolingCombo, CB_GETCURSEL, 0, 0);
            cooling_type ct = cooling_type::EXPONENTIAL;
            switch (coolingIndex) 
            {
                case 0: ct = cooling_type::LINEAR; break;
                case 1: ct = cooling_type::EXPONENTIAL; break;
            }

            if (pSAAlgorithm) delete pSAAlgorithm;
            pSAAlgorithm = new SaAlgorithm(*p_cnf, *pInitialSolution);

            // Запускаем алгоритм
            Candidate result = pSAAlgorithm->execute(initial_temp, min_temp, cooling_rate, ct, iterations);

            // Сохраняем данные для графика
            g_sa_best_qualities = pSAAlgorithm->get_quality_history();
            g_graph_title = L"Алгоритм имитации отжига";

            // Выводим результат
            std::wstring message = L"Алгоритм завершен!\n";
            message += L"Лучшее решение: " + std::wstring(result.get_function().begin(), result.get_function().end()) + L"\n";
            message += L"Энергия: " + std::to_wstring(1.0 - result.get_quality());

            MessageBoxW(hWnd, message.c_str(), L"Результат выполнения", MB_OK);

            // Открываем окно с графиком
            HWND hGraphWnd = CreateWindowEx(
                0,
                _T("GraphWindowClass"),
                _T("График функции качества"),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                hWnd, NULL, NULL, NULL);

            if (hGraphWnd)
            {
                ShowWindow(hGraphWnd, SW_SHOW);
                UpdateWindow(hGraphWnd);
            }
        }
        break;
    }
    case WM_DESTROY:
    {
        if (pSAAlgorithm) delete pSAAlgorithm;
        if (p_cnf) delete p_cnf;
        if (pInitialSolution) delete pInitialSolution;
        if (hFont) DeleteObject(hFont);
        DestroyWindow(hWnd);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

double GetDlgItemDouble(HWND hDlg, int nIDDlgItem)
{
    wchar_t szText[256];
    GetDlgItemTextW(hDlg, nIDDlgItem, szText, 256);
    return _wtof(szText);
}

LRESULT CALLBACK GraphWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Gdiplus::Graphics graphics(hdc);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right;
        int height = rect.bottom;

        // Настройки отступов
        const int margin_left = 50;
        const int margin_bottom = 50;
        const int margin_top = 50;
        const int margin_right = 20;

        int graph_width = width - margin_left - margin_right;
        int graph_height = height - margin_top - margin_bottom;

        // Начало координат
        int x0 = margin_left;
        int y0 = height - margin_bottom;

        // Настройки стилей
        Gdiplus::Pen axisPen(Gdiplus::Color(255, 0, 0, 0), 2);
        Gdiplus::Pen graphPen(Gdiplus::Color(255, 0, 0, 255), 2);
        Gdiplus::Font font(L"Arial", 10);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));
        Gdiplus::Font titleFont(L"Arial", 14, Gdiplus::FontStyleBold);

        // Рисуем заголовок
        graphics.DrawString(g_graph_title.c_str(), -1, &titleFont, 
                          Gdiplus::PointF((width - g_graph_title.length() * 10) / 2.0f, 10.0f), &brush);

        // Рисуем оси
        graphics.DrawLine(&axisPen, x0, y0, x0 + graph_width, y0); // Ось X
        graphics.DrawLine(&axisPen, x0, y0, x0, margin_top);       // Ось Y

        const std::vector<double>* data = nullptr;
        bool isSimulatedAnnealing = false;

        if (g_graph_title == L"Генетический алгоритм") 
        {
            data = &g_ga_best_qualities;
            
            // Стандартные подписи для генетического алгоритма (0.0 - 1.0)
            for (int i = 0; i <= 10; ++i)
            {
                double y_val = i / 10.0;
                int y = y0 - static_cast<int>(y_val * graph_height);
                std::wstring label = std::to_wstring(y_val).substr(0, 4);
                graphics.DrawString(label.c_str(), -1, &font, Gdiplus::PointF((float)x0 - 45.0f, (float)y - 8), &brush);
                graphics.DrawLine(&axisPen, x0 - 5, y, x0, y);
            }
        } 
        else if (g_graph_title == L"Алгоритм имитации отжига") 
        {
            data = &g_sa_best_qualities;
            isSimulatedAnnealing = true;
            
            // Специальные подписи для алгоритма отжига (0 - 500 с шагом 100)
            for (int i = 0; i <= 5; ++i)
            {
                int temp = i * 100;
                int y = y0 - static_cast<int>((i / 5.0) * graph_height);
                std::wstring label = std::to_wstring(temp);
                graphics.DrawString(label.c_str(), -1, &font, Gdiplus::PointF((float)x0 - 45.0f, (float)y - 8), &brush);
                graphics.DrawLine(&axisPen, x0 - 5, y, x0, y);
            }
        }

        if (data && !data->empty())
        {
            size_t total_points = data->size();
            int label_step = std::max(1, static_cast<int>(total_points / 10));

            // Подписи оси X (итерации)
            for (size_t i = 0; i < total_points; i += label_step)
            {
                int x = x0 + static_cast<int>((i / (double)(total_points - 1)) * graph_width);
                std::wstring label = std::to_wstring(i);
                graphics.DrawString(label.c_str(), -1, &font, Gdiplus::PointF((float)x - 10.0f, (float)y0 + 5.0f), &brush);
                graphics.DrawLine(&axisPen, x, y0, x, y0 + 5);
            }

            // Рисуем график
            for (size_t i = 1; i < total_points; ++i)
            {
                double x1 = x0 + ((i - 1) / (double)(total_points - 1)) * graph_width;
                double y1 = isSimulatedAnnealing 
                    ? y0 - ((*data)[i - 1] / 500.0) * graph_height  // Масштабирование для отжига (0-500)
                    : y0 - (*data)[i - 1] * graph_height;           // Масштабирование для ГА (0.0-1.0)

                double x2 = x0 + (i / (double)(total_points - 1)) * graph_width;
                double y2 = isSimulatedAnnealing 
                    ? y0 - ((*data)[i] / 500.0) * graph_height      // Масштабирование для отжига (0-500)
                    : y0 - (*data)[i] * graph_height;               // Масштабирование для ГА (0.0-1.0)

                graphics.DrawLine(&graphPen, (float)x1, (float)y1, (float)x2, (float)y2);
            }
        }
        else
        {
            MessageBoxW(hWnd, L"Нет данных для построения графика", L"Ошибка", MB_OK);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        Gdiplus::GdiplusShutdown(0);
        DestroyWindow(hWnd);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void RegisterGraphWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GraphWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _T("GraphWindowClass");

    RegisterClassEx(&wc);
}