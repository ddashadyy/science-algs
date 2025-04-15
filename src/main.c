#define WIN32_LEAN_AND_MEAN
#define _UNICODE
#define UNICODE


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "cnf.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


HWND hEdit;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) 
{
    _CRT_UNUSED(hPrevInstance);
    _CRT_UNUSED(szCmdLine);

    SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), SORT_DEFAULT));
    SetConsoleOutputCP(1251);

    static TCHAR szAppName[] = TEXT("CNF Generator");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) 
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,
        TEXT("CNF and Candidates Generator"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 200,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_CREATE: 
    {
        CreateWindow(TEXT("STATIC"), TEXT("Введите количество переменных:"),
            WS_VISIBLE | WS_CHILD,
            20, 20, 200, 20,
            hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        hEdit = CreateWindow(TEXT("EDIT"), NULL,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            20, 45, 100, 20,
            hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        
        CreateWindow(TEXT("BUTTON"), TEXT("Сгенерировать кандидатов"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 80, 200, 30,
            hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindow(TEXT("BUTTON"), TEXT("Сгенерировать КНФ"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 120, 200, 30,
            hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        return 0;
    }
    case WM_COMMAND: 
    {
        if (LOWORD(wParam) == 1 || LOWORD(wParam) == 2) {
            int var_count = 0;
            TCHAR buffer[256];
            
            GetWindowText(hEdit, buffer, 256);
            var_count = _ttoi(buffer);

            if (var_count <= 0) 
            {
                MessageBox(hwnd, TEXT("Пожалуйста, введите корректное количество переменных (больше 0)"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
                return 0;
            }

            if (LOWORD(wParam) == 1) 
            {
                // Генерация кандидатов
                write_candidates_in_file(var_count);
                MessageBox(hwnd, TEXT("Кандидаты успешно сгенерированы!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
            } 
            else if (LOWORD(wParam) == 2) 
            {
                // Генерация КНФ
                write_cnf_in_file(var_count);
                MessageBox(hwnd, TEXT("КНФ успешно сгенерирована!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
            }
        }
        return 0;
    }
    case WM_DESTROY: 
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

