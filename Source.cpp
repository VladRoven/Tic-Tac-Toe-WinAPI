#include <windows.h>
#include "resource.h"
#include <cmath>
#include <iostream>


HINSTANCE hInst;
const int size = 20;

struct square
{
	int num;
	int x, y;
	char value;
};

square** arr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
void initArray();
int check();

int WINAPI WinMain(HINSTANCE hInstance, // дескриптор экземпляра приложения
	HINSTANCE hPrevInst, // не используем
	LPSTR lpCmdLine, // не используем
	int nCmdShow) // режим отображения окошка
{
	TCHAR szClassName[] = "tic_tac_toe"; // строка с именем класса
	HWND hMainWnd; // создаём дескриптор будущего окошка
	MSG msg; // создём экземпляр структуры MSG для обработки сообщений
	WNDCLASSEX wc; // создаём экземпляр, для обращения к членам класса WNDCLASSEX
	wc.cbSize = sizeof(wc); // размер структуры (в байтах)
	wc.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
	wc.lpfnWndProc = WndProc; // указатель на пользовательскую функцию
	wc.lpszMenuName = NULL; // указатель на имя меню (у нас его нет)
	wc.lpszClassName = szClassName; // указатель на имя класса
	wc.cbWndExtra = NULL; // число освобождаемых байтов в конце структуры
	wc.cbClsExtra = NULL; // число освобождаемых байтов при создании экземпляра приложения
	wc.hIcon = LoadIcon(NULL, IDI_INFORMATION); // декриптор пиктограммы
	wc.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // дескриптор маленькой пиктограммы (в трэе)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // дескриптор кисти для закраски фона окна
	wc.hInstance = hInst;
	if (!RegisterClassEx(&wc)) 
	{
		MessageBox(NULL, "Не получилось зарегистрировать класс!", "Ошибка", MB_OK);
		return NULL;
	}

	hMainWnd = CreateWindow(
		szClassName,
		"Крестики нолики | Сейчас ходят: X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		NULL,
		600,
		600,
		(HWND)NULL,
		NULL,
		HINSTANCE(hInst),
		NULL); 

	if (!hMainWnd) 
	{
		MessageBox(NULL, "Не получилось создать окно!", "Ошибка", MB_OK);
		return NULL;
	}
	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);
	while (GetMessage(&msg, NULL, NULL, NULL)) 
	{
		hInst = hInstance;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	POINT pt;
	TCHAR textBuffer[100];
	TEXTMETRIC tm;

	static int sizeX;
	static int sizeY;
	static int textHeight;
	static double sX;
	static double sY;
	static bool flag = true;
	static int player = 1;

	switch (message)
	{
	case WM_CREATE:
		initArray();
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rt);
		SetBkMode(hdc, TRANSPARENT);
		GetTextMetrics(hdc, &tm);
		textHeight = tm.tmHeight;

		sizeX = rt.right;
		sizeY = rt.bottom;
		sX = double(sizeX) / double(size);
		sY = double(sizeY) / double(size);

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				Rectangle(hdc, int(arr[i][j].x - 1) * sX, int(arr[i][j].y - 1) * sY, int(arr[i][j].x) * sX, int(arr[i][j].y) * sY);
				TextOut(hdc, (((arr[i][j].x - 1) * sX + (arr[i][j].x) * sX) / 2) - (textHeight / 2) + 3, (((arr[i][j].y - 1) * sY + (arr[i][j].y) * sY) / 2) - (textHeight / 2), textBuffer, wsprintf(textBuffer, "%c", arr[i][j].value));
			}
		}

		DeleteDC(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONUP:
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		/*wsprintf(textBuffer, "X: %d Y: %d", pt.x, pt.y);
		MessageBox(hWnd, textBuffer, "Координаты курсора", MB_OK);*/
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if ((pt.x >= arr[i][j].x && pt.x <= arr[i][j].x * sX) && (pt.y >= arr[i][j].y && pt.y <= arr[i][j].y * sY))
				{
					if (arr[i][j].value == ' ')
					{
						if (player == 1)
						{
							arr[i][j].value = 'X';
							player = 2;

							LoadString(hInst, IDS_PLAYER2, textBuffer, 100);
							SetWindowText(hWnd, textBuffer);
						}
						else if (player == 2)
						{
							arr[i][j].value = 'O';
							player = 1;

							LoadString(hInst, IDS_PLAYER1, textBuffer, 100);
							SetWindowText(hWnd, textBuffer);
						}

						flag = false;
						break;
					}
					else
					{
						flag = false;
						break;
					}
				}
			}
			if (!flag)
			{
				flag = true;
				break;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return NULL;
}

void initArray()
{
	int clk = 0;
	arr = new square * [size];

	for (int i = 0; i < size; i++)
		arr[i] = new square[size];

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			arr[i][j].num = clk++;
			arr[i][j].value = ' ';
			arr[i][j].y = i + 1;
			arr[i][j].x = j + 1;
		}
	}
}

int check()
{

	return 1;
}