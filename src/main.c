#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Точка входа приложения
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) 
{

    _CRT_UNUSED(hPrevInstance);
    _CRT_UNUSED(szCmdLine);

    
    static TCHAR szAppName[] = TEXT("HelloWin");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    // Заполняем структуру класса окна
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

    // Регистрируем класс окна
    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"), 
                   TEXT("Ошибка"), MB_ICONERROR);
        return 0;
    }

    // Создаем окно
    hwnd = CreateWindow(
        szAppName,                  // Имя класса окна
        TEXT("Hello, WinAPI!"),     // Заголовок окна
        WS_OVERLAPPEDWINDOW,        // Стиль окна
        CW_USEDEFAULT,              // Позиция X
        CW_USEDEFAULT,              // Позиция Y
        CW_USEDEFAULT,             // Ширина
        CW_USEDEFAULT,             // Высота
        NULL,                      // Родительское окно
        NULL,                      // Меню
        hInstance,                  // Дескриптор экземпляра
        NULL                        // Дополнительные параметры
    );

    // Отображаем и обновляем окно
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Функция обработки сообщений
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        DrawText(hdc, TEXT("Hello, WinAPI!"), -1, &rect,
            DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}