#define WIN32_LEAN_AND_MEAN
#define _UNICODE
#define UNICODE

#include <windows.h>
#include <stdio.h>
#include "cnf.h"


// Идентификаторы элементов управления
#define ID_BUTTON 1
#define ID_EDIT 2
#define ID_STATIC 3

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) 
{

    _CRT_UNUSED(hPrevInstance);
    _CRT_UNUSED(szCmdLine);

    // Установка русского языка
    SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), SORT_DEFAULT));
    SetConsoleOutputCP(1251);

    static TCHAR szAppName[] = TEXT("Генератор КНФ");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    // Регистрация класса окна
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"), 
                  TEXT("Ошибка"), MB_ICONERROR);
        return 0;
    }

    // Создание главного окна
    hwnd = CreateWindow(
        szAppName,
        TEXT("Генератор случайных булевых функций в КНФ"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        450, 200,
        NULL, NULL, hInstance, NULL);

    // Создание статического текста (метки)
    CreateWindow(
        TEXT("STATIC"),
        TEXT("Введите количество переменных функции (1-10):"),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        20, 20, 300, 20,
        hwnd, (HMENU)ID_STATIC, hInstance, NULL);

    // Создание поля ввода
    CreateWindow(
        TEXT("EDIT"),
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL,
        20, 50, 100, 25,
        hwnd, (HMENU)ID_EDIT, hInstance, NULL);

    // Создание кнопки
    CreateWindow(
        TEXT("BUTTON"),
        TEXT("Сгенерировать КНФ"),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, 90, 150, 30,
        hwnd, (HMENU)ID_BUTTON, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // Цикл сообщений
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Функция обработки сообщений
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static TCHAR szBuffer[256];
    
    switch (message)
    {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BUTTON)
            {
                GetDlgItemText(hwnd, ID_EDIT, szBuffer, sizeof(szBuffer)/sizeof(TCHAR));
                
                int varCount = _wtoi(szBuffer);
                if (varCount <= 0 || varCount > 10)
                {
                    MessageBox(hwnd, 
                        TEXT("Пожалуйста, введите число от 1 до 10"), 
                        TEXT("Некорректный ввод"), 
                        MB_ICONWARNING);
                }
                else
                {
                    write_cnf_in_file(varCount);
                    
                    TCHAR messageText[100];
                    wsprintf(messageText, 
                        TEXT("Сгенерированы 100 КНФ с %d переменными\nРезультат сохранен в cnf_file.txt"), 
                        varCount);
                    MessageBox(hwnd, messageText, TEXT("Генерация завершена"), MB_OK);
                }
            }
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}