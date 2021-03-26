#include <windows.h>
#include "resource.h"
#include <cmath>
#include <iostream>


HINSTANCE hInst;
COLORREF colorX;
COLORREF colorO;
COLORREF colorBgrnd;
const int size = 20;
const int block = 5;

struct square
{
	int x, y;
	char value;
	COLORREF clr;
};

struct showWinner
{
	int i, j;
};

showWinner t_arr[5];
square** arr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
void initArray();
int checkWinner();
bool chekLines(char, int, int);
bool chekDiagonal(char, int, int);
void colorPicker(HWND, char);
bool chekDraw();

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
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); // указатель на имя меню (у нас его нет)
	wc.lpszClassName = szClassName; // указатель на имя класса
	wc.cbWndExtra = NULL; // число освобождаемых байтов в конце структуры
	wc.cbClsExtra = NULL; // число освобождаемых байтов при создании экземпляра приложения
	wc.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE (IDI_ICON_TTT)); // декриптор пиктограммы
	wc.hIconSm = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_TTT)); // дескриптор маленькой пиктограммы (в трэе)
	wc.hCursor = LoadCursor(NULL, IDC_HAND); // дескриптор курсора
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
	HBRUSH hbr;
	HPEN hPen;

	static int sizeX;
	static int sizeY;
	static int textHeight;
	static double sX;
	static double sY;
	static bool flag = true;
	static int player = 1;
	static bool win = false;

	switch (message)
	{
	case WM_CREATE:
		initArray();
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CHANGEX:
			colorPicker(hWnd, 'X');
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case ID_CHANGEO:
			colorPicker(hWnd, 'O');
			InvalidateRect(hWnd, NULL, TRUE);
			break;
			
		case ID_CHANGE_BGRND:
			colorPicker(hWnd, '-');
			InvalidateRect(hWnd, NULL, TRUE);
			break;
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
				hbr = (colorBgrnd != 0) ? CreateSolidBrush(colorBgrnd) : CreateSolidBrush(RGB(255, 255, 255));
				hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

				SelectObject(hdc, hbr);
				SelectObject(hdc, hPen);
				
				if (arr[i][j].clr != -1)
				{
					hPen = CreatePen(PS_SOLID, 3, arr[i][j].clr);
					SelectObject(hdc, hPen);
				}

				Rectangle(hdc, int(arr[i][j].x - 1) * sX, int(arr[i][j].y - 1) * sY, int(arr[i][j].x) * sX, int(arr[i][j].y) * sY);
				DeleteObject(hbr);
				DeleteObject(hPen);

				if (arr[i][j].value == 'X')
					if (colorX != 0)
						SetTextColor(hdc, colorX);
					else
						SetTextColor(hdc, RGB(0, 0, 0));
				else if (arr[i][j].value == 'O')
					if (colorO != 0)
						SetTextColor(hdc, colorO);
					else
						SetTextColor(hdc, RGB(0, 0, 0));

				TextOut(hdc, (((arr[i][j].x - 1) * sX + (arr[i][j].x) * sX) / 2) - (textHeight / 2) + 3, (((arr[i][j].y - 1) * sY + (arr[i][j].y) * sY) / 2) - (textHeight / 2), textBuffer, wsprintf(textBuffer, "%c", arr[i][j].value));
			}
		}

		DeleteDC(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);

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
						InvalidateRect(hWnd, NULL, TRUE);
						
						if (checkWinner() == 1)
						{
							InvalidateRect(hWnd, NULL, TRUE);
							MessageBox(hWnd, "Выиграли X!", "Конец игры", MB_OK);
							win = true;
						}
						else if (checkWinner() == 2)
						{
							InvalidateRect(hWnd, NULL, TRUE);
							MessageBox(hWnd, "Выиграли O!", "Конец игры", MB_OK);
							win = true;
							
						}
						else if (checkWinner() == 3)
						{
							MessageBox(hWnd, "Ничья!", "Конец игры", MB_OK);
							win = true;

						}

						if (win)
						{
							win = false;
							initArray();
							player = 1;
							LoadString(hInst, IDS_PLAYER1, textBuffer, 100);
							SetWindowText(hWnd, textBuffer);
							colorO = 0;
							colorBgrnd = 0;
							colorX = 0;
							InvalidateRect(hWnd, NULL, TRUE);
						}

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
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return NULL;
}

void initArray()
{
	arr = new square * [size];

	for (int i = 0; i < size; i++)
		arr[i] = new square[size];

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			arr[i][j].value = ' ';
			arr[i][j].y = i + 1;
			arr[i][j].x = j + 1;
			arr[i][j].clr = -1;
		}
	}
}

int checkWinner()
{
	int pWinner = 0;

	for (int i = 0; i <= size - block; i++)
	{
		for (int j = 0; j <= size - block; j++)
		{
			if (chekLines('X', i, j))
				pWinner = 1;
			else if (chekLines('O', i, j))
				pWinner = 2;
			else if (chekDiagonal('X', i, j))
				pWinner = 1;
			else if (chekDiagonal('O', i, j))
				pWinner = 2;
			else if (chekDraw())
				pWinner = 3;
		}
	}

	return pWinner;
}

bool chekLines(char symbol, int x, int y)
{
	int counter = 0;

	for (int i = 0; i < 5; i++)
	{
		t_arr[i].i = -1;
		t_arr[i].j = -1;
	}

	for (int i = x; i < x + block; i++)
	{
		for (int j = y; j < y + block; j++)
		{
			if (arr[i][j].value == symbol)
			{
				t_arr[counter].i = i;
				t_arr[counter].j = j;
				counter++;
			}
			else
			{
				counter = 0;
				break;
			}
		}

		if (counter == 5)
		{
			for (int i = 0; i < 5; i++)
			{
				arr[t_arr[i].i][t_arr[i].j].clr = RGB(255, 0, 0);
			}
			return true;
		}
	}

	for (int i = y; i < y + block; i++)
	{
		for (int j = x; j < x + block; j++)
		{
			if (arr[j][i].value == symbol)
			{
				t_arr[counter].i = j;
				t_arr[counter].j = i;
				counter++;
			}
			else
			{
				counter = 0;
				break;
			}
		}

		if (counter == 5)
		{
			for (int i = 0; i < 5; i++)
			{
				arr[t_arr[i].i][t_arr[i].j].clr = RGB(255, 0, 0);
			}
			return true;
		}
	}

	if (counter == 5)
	{
		for (int i = 0; i < 5; i++)
		{
			arr[t_arr[i].i][t_arr[i].j].clr = RGB(255, 0, 0);
		}
		return true;
	}

	return false;
}

bool chekDiagonal(char symbol, int x, int y)
{
	int counter = 0;
	int temp = y;

	for (int i = 0; i < 5; i++)
	{
		t_arr[i].i = -1;
		t_arr[i].j = -1;
	}

	for (int i = x; i < x + block; i++)
	{
		if (arr[i][temp++].value == symbol)
		{
			t_arr[counter].i = i;
			t_arr[counter].j = --temp;
			temp++;
			counter++;
		}
		else
		{
			counter = 0;
			break;
		}
	}

	if (counter == 5)
	{
		for (int i = 0; i < 5; i++)
		{
			arr[t_arr[i].i][t_arr[i].j].clr = RGB(255, 0, 0);
		}
		return true;
	}
	else
	{
		temp = y + block;
		for (int i = x; i < x + block; i++)
		{
			if (arr[i][temp--].value == symbol)
			{
				t_arr[counter].i = i;
				t_arr[counter].j = ++temp;
				temp--;
				counter++;
			}
			else
			{
				counter = 0;
				break;
			}
		}

		if (counter == 5)
		{
			for (int i = 0; i < 5; i++)
			{
				arr[t_arr[i].i][t_arr[i].j].clr = RGB(255, 0, 0);
			}
			return true;
		}
	}

	return false;
}

void colorPicker(HWND hWnd, char symbol)
{
	CHOOSECOLOR cc;
	DWORD dColors[3] = { 255,255,255 };

	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hWnd;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = (LPSTR)NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = (symbol == 'X') ? colorX : ((symbol == 'O') ? colorO : colorBgrnd);

	if (ChooseColor(&cc))
		if (symbol == 'X')
			colorX = cc.rgbResult;
		else if (symbol == 'O')
			colorO = cc.rgbResult;
		else if (symbol == '-')
			colorBgrnd = cc.rgbResult;
		
}

bool chekDraw()
{
	int counter = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (arr[i][j].value == ' ')
				++counter;
		}
	}
	
	if (counter == 0)
		return true;

	return false;
}