#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500 // for VK_OEM constants
#include <windows.h>
#include <commdlg.h>

#include "id.h"
#include "emu.h"

HWND hwndMessage;
HWND hwndMain, hwndGfx, hwndKeys;
HMENU hMenu;

LRESULT CALLBACK message_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_USER) {
		switch (wParam) {
			case ID_SAVE_FLASH:
				flash_save_changes();
				break;
			case ID_DEBUGGER:
				debugger();
				break;
			case ID_RESET:
				cpu_events |= EVENT_RESET;
				break;
			case ID_CONNECT:
				usblink_connect();
				break;
			case ID_SEND_DOCUMENT:
				usblink_put_file((char *)lParam);
				break;
			case ID_DISCONNECT:
				usblink_disconnect();
				break;
			case ID_SEND_TI84_FILE:
				send_file((char *)lParam);
				break;
		}
	}
	return 0;
}

static struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[16];
} bmi;

LRESULT CALLBACK gfx_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int i;
	switch (uMsg) {
	case WM_CREATE:
		bmi.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth     = 320;
		bmi.bmiHeader.biHeight    = -240;
		bmi.bmiHeader.biPlanes    = 1;
		bmi.bmiHeader.biBitCount  = 4;
		for (i = 0; i < 16; i++)
			*(DWORD *)&bmi.bmiColors[i] = 0x111111 * i;
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SetDIBitsToDevice(hdc, 0, 0, 320, 240, 0, 0, 0, 240,
		                  lcd_framebuffer, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);
		EndPaint(hWnd, &ps);
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK keys_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int row, col;
	RECT rc;
	static const char key_names[2][8][12][6] = {
		{{ "ret", "enter","space","(-)", "Z",   ".",   "Y",   "0",  "X",    "---", "theta","on"    },
		 { ",",   "+",    "W",    "3",   "V",   "2",   "U",   "1",  "T",    "e^x", "pi",   "click" },
		 { "?",   "-",    "S",    "6",   "R",   "5",   "Q",   "4",  "P",    "10^x","EE",   "???"   },
		 { ":",   "*",    "O",    "9",   "N",   "8",   "M",   "7",  "L",    "x^2", "i",    "???"   },
		 { "\"",  "/",    "K",    "tan", "J",   "cos", "I",   "sin","H",    "^",   ">",    "---"   },
		 { "'",   "cat",  "G",    ")",   "F",   "(",   "E",   "var","D",    "caps","<",    "---"   },
		 { "flag","???",  "C",    "home","B",   "menu","A",   "esc","|",    "tab", "=",    "---"   },
		 { "up",  "---",  "right","---", "down","---", "left","---","clear","ctrl","=",    "---"   }},
		{{ "down", "left", "right","up",  "---", "---",  "---",  "---",  "---","---","---","on" },
		 { "enter","+",    "-",    "*",   "/",   "^",    "clear","---",  "---","---","---","---" },
		 { "(-)",  "3",    "6",    "9",   ")",   "tan",  "vars", "---",  "---","---","---","---" },
		 { ".",    "2",    "5",    "8",   "(",   "cos",  "prgm", "stat", "---","---","---","---" },
		 { "0",    "1",    "4",    "7",   ",",   "sin",  "apps", "X",    "---","---","---","---" },
		 { "---",  "sto",  "ln",   "log", "x^2", "x^-1", "math", "alpha","---","---","---","---" },
		 { "graph","trace","zoom", "wind","y=",  "2nd",  "mode", "del",  "---","---","---","---" },
		 { "---",  "---",  "---",  "---", "---", "---",  "---",  "---",  "---","---","---","---" }}
	};
	static const struct key_desc {
		BYTE vk_code;
		BYTE nspire_key;
		BYTE ti84_key;
	} key_table[] = {
		{ VK_RETURN,    0x01, 0x10 },
		{ VK_SPACE,     0x02, 0x40 },
		{ VK_OEM_MINUS, 0x03, 0x20 },
		{ 'Z',          0x04, 0x31 },
		{ VK_DECIMAL,   0x05, 0x30 },
		{ VK_OEM_PERIOD,0x05, 0x30 },
		{ 'Y',          0x06, 0x41 },
		{ '0',          0x07, 0x40 },
		{ VK_NUMPAD0,   0x07, 0x40 },
		{ 'X',          0x08, 0x51 },
		{ VK_OEM_COMMA, 0x10, 0x44 },
		{ VK_ADD,       0x11, 0x11 },
		{ 'W',          0x12, 0x12 },
		{ VK_NUMPAD3,   0x13, 0x21 },
		{ '3',          0x13, 0x21 },
		{ 'V',          0x14, 0x22 },
		{ VK_NUMPAD2,   0x15, 0x31 },
		{ '2',          0x15, 0x31 },
		{ 'U',          0x16, 0x32 },
		{ VK_NUMPAD1,   0x17, 0x41 },
		{ '1',          0x17, 0x41 },
		{ 'T',          0x18, 0x42 },
		{ VK_OEM_2,     0x20, 0x14 }, /* ? / */
		{ VK_SUBTRACT,  0x21, 0x12 },
		{ 'S',          0x22, 0x52 },
		{ VK_NUMPAD6,   0x23, 0x22 },
		{ '6',          0x23, 0x22 },
		{ 'R',          0x24, 0x13 },
		{ '5',          0x25, 0x32 },
		{ VK_NUMPAD5,   0x25, 0x32 },
		{ 'Q',          0x26, 0x23 },
		{ '4',          0x27, 0x42 },
		{ VK_NUMPAD4,   0x27, 0x42 },
		{ 'P',          0x28, 0x33 },
		{ VK_OEM_1,     0x30, 0xFF }, /* : ; */
		{ VK_MULTIPLY,  0x31, 0x13 },
		{ 'O',          0x32, 0x43 },
		{ VK_NUMPAD9,   0x33, 0x23 },
		{ '9',          0x33, 0x23 },
		{ 'N',          0x34, 0x53 },
		{ '8',          0x35, 0x33 },
		{ VK_NUMPAD8,   0x35, 0x33 },
		{ 'M',          0x36, 0x14 },
		{ '7',          0x37, 0x43 },
		{ VK_NUMPAD7,   0x37, 0x43 },
		{ 'L',          0x38, 0x24 },
		{ VK_OEM_7,     0x40, 0xFF }, /* " ' */
		{ VK_DIVIDE,    0x41, 0x14 },
		{ 'K',          0x42, 0x34 },
		{ 'J',          0x44, 0x44 },
		{ 'I',          0x46, 0x54 },
		{ 'H',          0x48, 0x15 },
		{ 'G',          0x52, 0x25 },
		{ VK_OEM_6,     0x53, 0x34 }, /* [ { */
		{ 'F',          0x54, 0x35 },
		{ VK_OEM_4,     0x55, 0x24 }, /* ] } */
		{ 'E',          0x56, 0x45 },
		{ 'D',          0x58, 0x55 },
		{ VK_RSHIFT,    0x59, 0x16 },
		{ VK_LSHIFT,    0x59, 0x65 },
		{ 'C',          0x62, 0x36 },
		{ VK_HOME,      0x63, 0x56 },
		{ 'B',          0x64, 0x46 },
		{ 'A',          0x66, 0x56 },
		{ VK_ESCAPE,    0x67, 0x66 },
		{ VK_TAB,       0x69, 0xFF },
		{ VK_OEM_PLUS,  0x6A, 0x47 },
		{ VK_UP,        0x70, 0x03 },
		{ VK_RIGHT,     0x72, 0x02 },
		{ VK_DOWN,      0x74, 0x00 },
		{ VK_LEFT,      0x76, 0x01 },
		{ VK_BACK,      0x78, 0xFF },
		{ VK_CONTROL,   0x79, 0x57 },
		{ VK_INSERT,    0xFF, 0x26 },
		{ VK_NEXT,      0xFF, 0x36 },
		{ VK_END,       0xFF, 0x37 },
		{ VK_PRIOR,     0xFF, 0x46 },
		{ VK_F5,        0xFF, 0x60 },
		{ VK_F4,        0xFF, 0x61 },
		{ VK_F3,        0xFF, 0x62 },
		{ VK_F2,        0xFF, 0x63 },
		{ VK_F1,        0xFF, 0x64 },
		{ VK_DELETE,    0xFF, 0x67 },
	};
	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UINT align = SetTextAlign(hdc, TA_CENTER);
		for (row = 0; row < 9; row++) {
			for (col = 0; col < 16; col++) {
				COLORREF tc, bc;
				const char *str;
				if (row < 8 && col < 12) 
					str = key_names[emulate_ti84_keypad][row][col];
				else
					str = "---";
				rc.left = col * 30; rc.right = rc.left + 30;
				rc.top = row * 20; rc.bottom = rc.top + 20;
				int is_down = key_map[row] & (1 << col);
				if (is_down) {
					tc = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					bc = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				}
				ExtTextOut(hdc, rc.left + 15, rc.top, ETO_OPAQUE, &rc, str, strlen(str), NULL);
				if (is_down) {
					SetTextColor(hdc, tc);
					SetBkColor(hdc, bc);
				}
			}
		}
		SetTextAlign(hdc, align);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		row = HIWORD(lParam) / 20;
		col = LOWORD(lParam) / 30;
		if (uMsg == WM_LBUTTONDOWN)
			key_map[row] |= 1 << col;
		else
			key_map[row] &= ~(1 << col);
		goto update;
	case WM_KEYDOWN:
	case WM_KEYUP: {
		const struct key_desc *p = key_table;
		if (wParam == VK_SHIFT) {
			/* distinguish between left and right shift */
			wParam = MapVirtualKey(lParam >> 16 & 0xFF, 3);
		}
		for (; p < (key_table + (sizeof(key_table) / sizeof(*key_table))); p++) {
			if (p->vk_code == wParam) {
				row = (&p->nspire_key)[emulate_ti84_keypad] >> 4;
				col = (&p->nspire_key)[emulate_ti84_keypad] & 15;
				if (row >= 9)
					break;
				if (uMsg == WM_KEYDOWN)
					key_map[row] |= 1 << col;
				else
					key_map[row] &= ~(1 << col);
update:
				rc.left = col * 30; rc.right = rc.left + 30;
				rc.top = row * 20; rc.bottom = rc.top + 20;
				InvalidateRect(hWnd, &rc, FALSE);
				break;
			}
		}
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK emu_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_KEYUP:
		// Redirect to keypad window
		return keys_wnd_proc(hwndKeys, uMsg, wParam, lParam);
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_SAVE_FLASH:
		case ID_DEBUGGER:
		case ID_RESET:
		case ID_CONNECT:
		case ID_DISCONNECT:
			PostMessage(hwndMessage, WM_USER, LOWORD(wParam), 0);
			break;
		case ID_THROTTLE:
			CheckMenuItem(hMenu, ID_THROTTLE, MF_BYCOMMAND | turbo_mode * MF_CHECKED);
			turbo_mode ^= 1;
			break;
		case ID_SHOW_SPEED:
			show_speed ^= 1;
			CheckMenuItem(hMenu, ID_SHOW_SPEED, MF_BYCOMMAND | show_speed * MF_CHECKED);
			if (!show_speed)
				SetWindowText(hwndMain, "nspire_emu");
			break;
		case ID_SCREENSHOT:
			if (OpenClipboard(hWnd)) {
				EmptyClipboard();
				HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, 
					sizeof(bmi) + (320 * 240 / 2));
				if (hglb) {
					BYTE *p = GlobalLock(hglb);
					memcpy(p, &bmi, sizeof(bmi));
					/* Some programs can't paste right-side-up bitmaps,
					 * so turn it upside down */
					((BITMAPINFOHEADER *)p)->biHeight = 240;
					p += sizeof(bmi);
					BYTE *in_row = lcd_framebuffer + (320 * 239 / 2);
					while (in_row >= lcd_framebuffer) {
						memcpy(p, in_row, 320 / 2);
						p += 320 / 2;
						in_row -= 320 / 2;
					}
					GlobalUnlock(hglb);
					SetClipboardData(CF_DIB, hglb);
				}
				CloseClipboard();
			}
			break;
		case ID_SEND_DOCUMENT: {
			char filename[MAX_PATH];
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			filename[0] = '\0';
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "TI-Nspire Documents (*.tns)\0*.tns\0"
			                  "All Files\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn)) {
				DWORD result;
				if (!SendMessageTimeout(hwndMessage, WM_USER, ID_SEND_DOCUMENT, (LPARAM)filename,
					SMTO_NORMAL, 1000, &result))
				{
					MessageBox(hWnd, "Can't send file while in debugger", NULL, MB_OK);
				}
			}
			break;
		}
		case ID_SEND_TI84_FILE: {
			char filename[MAX_PATH];
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			filename[0] = '\0';
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "TI-84+ Files (*.8xp, and so on)\0*.8x?;*.83?;*.82?\0"
			                  "All Files\0*.*\0";
			ofn.lpstrFile = filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn)) {
				DWORD result;
				if (!SendMessageTimeout(hwndMessage, WM_USER, ID_SEND_TI84_FILE, (LPARAM)filename,
					SMTO_NORMAL, 1000, &result))
				{
					MessageBox(hWnd, "Can't send file while in debugger", NULL, MB_OK);
				}
			}
			break;
		}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

DWORD CALLBACK gui_thread(LPVOID __attribute__((unused)) unused) {
	HANDLE hInstance = GetModuleHandle(NULL);
	WNDCLASS wc;
	MSG msg;
	RECT rc;
	HACCEL hAccel;

	memset(&wc, 0, sizeof wc);
	wc.lpfnWndProc = emu_wnd_proc;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.lpszClassName = "nspire_emu";
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MENU);
	if (!RegisterClass(&wc))
		exit(printf("RegisterClass failed\n"));
	wc.lpfnWndProc = gfx_wnd_proc;
	wc.hbrBackground = NULL;
	wc.lpszClassName = "nspire_gfx";
	wc.lpszMenuName = NULL;
	if (!RegisterClass(&wc))
		exit(printf("RegisterClass failed\n"));
	wc.lpfnWndProc = keys_wnd_proc;
	wc.lpszClassName = "nspire_keys";
	if (!RegisterClass(&wc))
		exit(printf("RegisterClass failed\n"));

	rc.left = 0;
	rc.top = 0;
	rc.right = 480;
	rc.bottom = 240 + 180;
	AdjustWindowRect(&rc, WS_VISIBLE | (WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX), TRUE);

	hwndMain = CreateWindow("nspire_emu", "nspire_emu",
		WS_VISIBLE | (WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, NULL, NULL);
	hMenu = GetMenu(hwndMain);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_ACCEL));

	hwndKeys = CreateWindow("nspire_keys", NULL, WS_VISIBLE | WS_CHILD,
		0, 240, 480, 180, hwndMain, NULL, NULL, NULL);

	hwndGfx = CreateWindow("nspire_gfx", NULL, WS_VISIBLE | WS_CHILD,
		(480 - 320) / 2, 0, 320, 240, hwndMain, NULL, NULL, NULL);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		if (TranslateAccelerator(hwndMain, hAccel, &msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	exiting = true;
	return msg.wParam;
}

void gui_initialize() {
	DWORD gui_thread_id;

	hwndMessage = CreateWindow("Static", NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	SetWindowLong(hwndMessage, GWL_WNDPROC, (LONG)message_wnd_proc);

	CreateThread(NULL, 0, gui_thread, NULL, 0, &gui_thread_id);
}

void get_messages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}
