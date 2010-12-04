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

char target_folder[256];

HWND hwndMessage;
HWND hwndMain, hwndGfx, hwndKeys;
HMENU hMenu;

LRESULT CALLBACK message_wnd_proc(__attribute__((unused)) HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_USER) {
		switch (wParam) {
			case ID_SAVE_STATE:
				save_state();
				break;
			case ID_SAVE_FLASH:
				flash_save_changes();
				break;
			case ID_SAVE_FLASH_AS:
				flash_save_as();
				break;
			case ID_RELOAD_STATE:
				reload_state();
				break;
			case ID_DEBUGGER:
				debugger(DBG_USER, 0);
				break;
			case ID_RESET:
				cpu_events |= EVENT_RESET;
				break;
			case ID_CONNECT:
				usblink_connect();
				break;
			case ID_SEND_DOCUMENT:
				usblink_put_file((char *)lParam, (char *)lParam + MAX_PATH);
				break;
			case ID_SEND_OS:
				usblink_send_os((char *)lParam);
				break;
			case ID_DISCONNECT:
				usblink_disconnect();
				break;
			case ID_SEND_TI84_FILE:
				send_file((char *)lParam);
				break;
			case ID_INCREASE_SPEED:
				if (throttle_delay == 1) break;
				throttle_timer_off();
				throttle_delay--;
				throttle_timer_on();
				break;
			case ID_DECREASE_SPEED:
				throttle_timer_off();
				throttle_delay++;
				throttle_timer_on();
				break;
		}
	} else if (uMsg == WM_USER + 1) {
		// Keypad state changed
		keypad_int_active |= 2;
		keypad_int_check();
	}
	return 0;
}

static struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[16];
} bmi;
static BYTE framebuffer[240][160];
LRESULT CALLBACK gfx_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int i;
	switch (uMsg) {
	case WM_CREATE:
		bmi.bmiHeader.biSize      = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth     = 320;
		bmi.bmiHeader.biHeight    = 240;
		bmi.bmiHeader.biPlanes    = 1;
		bmi.bmiHeader.biBitCount  = 4;
		for (i = 0; i < 16; i++)
			*(DWORD *)&bmi.bmiColors[i] = ~(0x111111 * i);
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		lcd_draw_frame(framebuffer);
		SetDIBitsToDevice(hdc, 0, 0, 320, 240, 0, 0, 0, 240,
		                  framebuffer, (BITMAPINFO *)&bmi, DIB_RGB_COLORS);
		EndPaint(hWnd, &ps);
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

#define KEY_WIDTH 30
#define KEY_HEIGHT 20
#define KEYPAD_WIDTH (KEY_WIDTH * KEYPAD_COLUMNS)
#define KEYPAD_HEIGHT (KEY_HEIGHT * KEYPAD_ROWS)
LRESULT CALLBACK keys_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int row, col, down;
	RECT rc;

	static const char key_names[NUM_KEYPAD_TYPES][8][11][5] = {
		{{ "ret", "enter","space","(-)", "Z",   ".",   "Y",   "0",  "X",  "on",   "theta" },
		 { ",",   "+",    "W",    "3",   "V",   "2",   "U",   "1",  "T",  "e^x",  "pi"    },
		 { "?",   "-",    "S",    "6",   "R",   "5",   "Q",   "4",  "P",  "10^x", "EE"    },
		 { ":",   "*",    "O",    "9",   "N",   "8",   "M",   "7",  "L",  "x^2",  "i"     },
		 { "\"",  "/",    "K",    "tan", "J",   "cos", "I",   "sin","H",  "^",    ">"     },
		 { "'",   "cat",  "G",    ")",   "F",   "(",   "E",   "var","D",  "shift","<"     },
		 { "flag","click","C",    "home","B",   "menu","A",   "esc","|",  "tab",  "---"   },
		 { "up",  "u+r",  "right","r+d", "down","d+l", "left","l+u","del","ctrl", "="     }},

		{{ "down", "left", "right","up",  "---", "---",  "---",  "---",  "---","on", "---" },
		 { "enter","+",    "-",    "*",   "/",   "^",    "clear","---",  "---","---","---" },
		 { "(-)",  "3",    "6",    "9",   ")",   "tan",  "vars", "---",  "---","---","---" },
		 { ".",    "2",    "5",    "8",   "(",   "cos",  "prgm", "stat", "---","---","---" },
		 { "0",    "1",    "4",    "7",   ",",   "sin",  "apps", "X",    "---","---","---" },
		 { "---",  "sto",  "ln",   "log", "x^2", "x^-1", "math", "alpha","---","---","---" },
		 { "graph","trace","zoom", "wind","y=",  "2nd",  "mode", "del",  "---","---","---" },
		 { "---",  "---",  "---",  "---", "---", "---",  "---",  "---",  "---","---","---" }},

		{{ "ret",  "enter","---",  "(-)", "space","Z",   "Y",   "0",  "?!",  "on",   "---"  },
		 { "X",    "W",    "V",    "3",   "U",    "T",   "S",   "1",  "pi",  "trig", "10^x" },
		 { "R",    "Q",    "P",    "6",   "O",    "N",   "M",   "4",  "EE",  "x^2",  "---"  },
		 { "L",    "K",    "J",    "9",   "I",    "H",   "G",   "7",  "/",   "e^x",  "---"  },
		 { "F",    "E",    "D",    "---", "C",    "B",   "A",   "=",  "*",   "^",    "---"  },
		 { "---",  "var",  "-",    ")",   ".",    "(",   "5",   "cat","frac","shift","---"  },
		 { "flag", "click","+",    "doc", "2",    "menu","8",   "esc","---", "tab",  "---"  },
		 { "right","r+u",  "up",   "u+l", "left", "l+d", "down","d+r","del", "ctrl", ","    }},

		{{ "ret",  "enter","---",  "(-)", "space","Z",   "Y",   "0",  "?!",   "on",  "---"  },
		 { "X",    "W",    "V",    "3",   "U",    "T",   "S",   "1",  "pi",   "trig","10^x" },
		 { "R",    "Q",    "P",    "6",   "O",    "N",   "M",   "4",  "EE",   "x^2", "---"  },
		 { "L",    "K",    "J",    "9",   "I",    "H",   "G",   "7",  "/",    "e^x", "---"  },
		 { "F",    "E",    "D",    "---", "C",    "B",   "A",   "=",  "*",    "^",   "---"  },
		 { "---",  "var",  "-",    ")",   ".",    "(",   "5",   "cat","frac", "del", "pad"  },
		 { "flag", "click","+",    "doc", "2",    "menu","8",   "esc","---",  "tab", "---"  },
		 { "---",  "---",  "---",  "---", "---",  "---", "---", "---","shift","ctrl",","    }},
	};
	static const struct key_desc {
		BYTE vk_code;
		BYTE ext; /* 1 = non-extended key only, 2 = extended key only, 3 = either */
		BYTE keypad_code[NUM_KEYPAD_TYPES];
	} key_table[] = {
		{ VK_RETURN,    2, { 0x00, 0x10, 0x00, 0x00 } },
		{ VK_RETURN,    1, { 0x01, 0x10, 0x01, 0x01 } },
		{ VK_SPACE,     3, { 0x02, 0x40, 0x04, 0x04 } },
		{ VK_OEM_MINUS, 3, { 0x03, 0x20, 0x03, 0x03 } },
		{ 'Z',          3, { 0x04, 0x31, 0x05, 0x05 } },
		{ VK_DECIMAL,   3, { 0x05, 0x30, 0x54, 0x54 } },
		{ VK_OEM_PERIOD,3, { 0x05, 0x30, 0x54, 0x54 } },
		{ 'Y',          3, { 0x06, 0x41, 0x06, 0x06 } },
		{ '0',          3, { 0x07, 0x40, 0x07, 0x07 } },
		{ 'X',          3, { 0x08, 0x51, 0x10, 0x10 } },
		{ VK_OEM_COMMA, 3, { 0x10, 0x44, 0x7A, 0x7A } },
		{ VK_ADD,       3, { 0x11, 0x11, 0x62, 0x62 } },
		{ 'W',          3, { 0x12, 0x12, 0x11, 0x11 } },
		{ '3',          3, { 0x13, 0x21, 0x13, 0x13 } },
		{ 'V',          3, { 0x14, 0x22, 0x12, 0x12 } },
		{ '2',          3, { 0x15, 0x31, 0x64, 0x64 } },
		{ 'U',          3, { 0x16, 0x32, 0x14, 0x14 } },
		{ '1',          3, { 0x17, 0x41, 0x17, 0x17 } },
		{ 'T',          3, { 0x18, 0x42, 0x15, 0x15 } },
		{ VK_OEM_2,     3, { 0x20, 0x14, 0x08, 0x08 } }, /* ? / */
		{ VK_SUBTRACT,  3, { 0x21, 0x12, 0x52, 0x52 } },
		{ 'S',          3, { 0x22, 0x52, 0x16, 0x16 } },
		{ '6',          3, { 0x23, 0x22, 0x23, 0x23 } },
		{ 'R',          3, { 0x24, 0x13, 0x20, 0x20 } },
		{ '5',          3, { 0x25, 0x32, 0x56, 0x56 } },
		{ 'Q',          3, { 0x26, 0x23, 0x21, 0x21 } },
		{ '4',          3, { 0x27, 0x42, 0x27, 0x27 } },
		{ 'P',          3, { 0x28, 0x33, 0x22, 0x22 } },
		{ VK_OEM_1,     3, { 0x30, 0xFF, 0x08, 0x08 } }, /* : ; */
		{ VK_MULTIPLY,  3, { 0x31, 0x13, 0x48, 0x48 } },
		{ 'O',          3, { 0x32, 0x43, 0x24, 0x24 } },
		{ '9',          3, { 0x33, 0x23, 0x33, 0x33 } },
		{ 'N',          3, { 0x34, 0x53, 0x25, 0x25 } },
		{ '8',          3, { 0x35, 0x33, 0x66, 0x66 } },
		{ 'M',          3, { 0x36, 0x14, 0x26, 0x26 } },
		{ '7',          3, { 0x37, 0x43, 0x37, 0x37 } },
		{ 'L',          3, { 0x38, 0x24, 0x30, 0x30 } },
		{ VK_OEM_7,     3, { 0x40, 0xFF, 0x08, 0x08 } }, /* " ' */
		{ VK_DIVIDE,    3, { 0x41, 0x14, 0x38, 0x38 } },
		{ 'K',          3, { 0x42, 0x34, 0x31, 0x31 } },
		{ 'J',          3, { 0x44, 0x44, 0x32, 0x32 } },
		{ 'I',          3, { 0x46, 0x54, 0x34, 0x34 } },
		{ 'H',          3, { 0x48, 0x15, 0x35, 0x35 } },
		{ 'G',          3, { 0x52, 0x25, 0x36, 0x36 } },
		{ VK_OEM_6,     3, { 0x53, 0x34, 0x53, 0x53 } }, /* [ { */
		{ 'F',          3, { 0x54, 0x35, 0x40, 0x40 } },
		{ VK_OEM_4,     3, { 0x55, 0x24, 0x55, 0x55 } }, /* ] } */
		{ 'E',          3, { 0x56, 0x45, 0x41, 0x41 } },
		{ 'D',          3, { 0x58, 0x55, 0x42, 0x42 } },
		{ VK_RSHIFT,    3, { 0x59, 0x16, 0x59, 0x78 } },
		{ VK_LSHIFT,    3, { 0x59, 0x65, 0x59, 0x78 } },
		{ VK_CLEAR,     3, { 0x61, 0xFF, 0x61, 0x61 } }, /* numeric keypad 5 */
		{ 'C',          3, { 0x62, 0x36, 0x44, 0x44 } },
		{ VK_HOME,      2, { 0x63, 0x56, 0x94, 0x94 } },
		{ 'B',          3, { 0x64, 0x46, 0x45, 0x45 } },
		{ 'A',          3, { 0x66, 0x56, 0x46, 0x46 } },
		{ VK_ESCAPE,    3, { 0x67, 0x66, 0x67, 0x67 } },
		{ VK_OEM_5,     3, { 0x68, 0xFF, 0x08, 0x08 } }, /* \ | */
		{ VK_TAB,       3, { 0x69, 0xFF, 0x69, 0x69 } },
		{ VK_UP,        3, { 0x70, 0x03, 0x72, 0x72 } },
		{ VK_PRIOR,     1, { 0x71, 0x46, 0x71, 0x71 } }, /* numeric keypad 9 */
		{ VK_RIGHT,     3, { 0x72, 0x02, 0x70, 0x70 } },
		{ VK_NEXT,      1, { 0x73, 0x36, 0x77, 0x77 } }, /* numeric keypad 3 */
		{ VK_DOWN,      3, { 0x74, 0x00, 0x76, 0x76 } },
		{ VK_END,       1, { 0x75, 0x37, 0x75, 0x75 } }, /* numeric keypad 1 */
		{ VK_LEFT,      3, { 0x76, 0x01, 0x74, 0x74 } },
		{ VK_HOME,      1, { 0x77, 0x56, 0x73, 0x73 } }, /* numeric keypad 7 */
		{ VK_BACK,      3, { 0x78, 0xFF, 0x78, 0x59 } },
		{ VK_CONTROL,   3, { 0x79, 0x57, 0x79, 0x79 } },
		{ VK_OEM_PLUS,  3, { 0x7A, 0x47, 0x47, 0x47 } }, /* = + */
		{ VK_INSERT,    3, { 0xFF, 0x26, 0xFF, 0xFF } },
		{ VK_NEXT,      2, { 0xFF, 0x36, 0xFF, 0xFF } },
		{ VK_END,       2, { 0xFF, 0x37, 0xFF, 0xFF } },
		{ VK_PRIOR,     2, { 0xFF, 0x46, 0xFF, 0xFF } },
		{ VK_F5,        3, { 0xFF, 0x60, 0xFF, 0xFF } },
		{ VK_F4,        3, { 0xFF, 0x61, 0xFF, 0xFF } },
		{ VK_F3,        3, { 0xFF, 0x62, 0xFF, 0xFF } },
		{ VK_F2,        3, { 0xFF, 0x63, 0xFF, 0xFF } },
		{ VK_F1,        3, { 0xFF, 0x64, 0xFF, 0xFF } },
		{ VK_DELETE,    3, { 0xFF, 0x67, 0xFF, 0xFF } },
	};
	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		UINT align = SetTextAlign(hdc, TA_CENTER);
		for (row = 0; row < KEYPAD_ROWS; row++) {
			for (col = 0; col < KEYPAD_COLUMNS; col++) {
				COLORREF tc = 0, bc = 0;
				const char *str;
				if (row < 8 && col < 11)
					str = key_names[keypad_type][row][col];
				else
					str = "---";
				int len = min(strlen(str), 5);
				rc.left = col * KEY_WIDTH; rc.right = rc.left + KEY_WIDTH;
				rc.top = row * KEY_HEIGHT; rc.bottom = rc.top + KEY_HEIGHT;
				int is_down = key_map[row] & (1 << col);
				if (is_down) {
					tc = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					bc = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				}
				ExtTextOut(hdc, rc.left + KEY_WIDTH / 2, rc.top, ETO_OPAQUE, &rc, str, len, NULL);
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
		row = HIWORD(lParam) / KEY_HEIGHT;
		col = LOWORD(lParam) / KEY_WIDTH;
		down = (wParam / MK_LBUTTON) & 1;
		goto update;
	case WM_KEYDOWN:
	case WM_KEYUP: {
		const struct key_desc *p = key_table;
		if (wParam == VK_SHIFT) {
			/* distinguish between left and right shift */
			wParam = MapVirtualKey(lParam >> 16 & 0xFF, 3);
		} else if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
			wParam += '0' - VK_NUMPAD0;
		}
		for (; p < (key_table + (sizeof(key_table) / sizeof(*key_table))); p++) {
			if (p->vk_code == wParam && p->ext & 1 << (lParam >> 24 & 1)) {
				row = p->keypad_code[keypad_type] >> 4;
				col = p->keypad_code[keypad_type] & 15;
				down = !(lParam >> 31);
update:
				if (row >= KEYPAD_ROWS)
					break;
				if ((key_map[row] >> col & 1) != down) {
					key_map[row] ^= 1 << col;
					PostMessage(hwndMessage, WM_USER + 1, 0, 0);

					rc.left = col * KEY_WIDTH; rc.right = rc.left + KEY_WIDTH;
					rc.top = row * KEY_HEIGHT; rc.bottom = rc.top + KEY_HEIGHT;
					InvalidateRect(hWnd, &rc, FALSE);
				}
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

BOOL CALLBACK folder_dlg_proc(HWND hWnd, UINT uMsg, __attribute__((unused)) WPARAM wParam, __attribute__((unused)) LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			SetDlgItemText(hWnd, IDC_TARGET_FOLDER, target_folder);
			return TRUE; // set default focus
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					GetDlgItemText(hWnd, IDC_TARGET_FOLDER, target_folder, sizeof target_folder);
					// fallthrough
				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;
			}
			return FALSE;
	}
	return FALSE;
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
		case ID_SAVE_STATE:
		case ID_RELOAD_STATE:
		case ID_SAVE_FLASH:
		case ID_DEBUGGER:
		case ID_INCREASE_SPEED:
		case ID_DECREASE_SPEED:
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
					lcd_draw_frame((void *)(p + sizeof(bmi)));
					GlobalUnlock(hglb);
					SetClipboardData(CF_DIB, hglb);
				}
				CloseClipboard();
			}
			break;
		case ID_SET_FOLDER:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SET_FOLDER),
				hWnd, folder_dlg_proc);
			break;
		case ID_SAVE_FLASH_AS: {
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = flash_filename;
			ofn.nMaxFile = MAX_PATH;
			if (GetSaveFileName(&ofn)) 
				SendMessageCallback(hwndMessage, WM_USER, LOWORD(wParam), 0, 0, 0);
			break;
		}
		case ID_SEND_DOCUMENT:
		case ID_SEND_OS:
		case ID_SEND_TI84_FILE: {
			struct {
				char filename[MAX_PATH];
				char folder[256];
			} lParam;
			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			lParam.filename[0] = '\0';
			ofn.lStructSize = sizeof ofn;
			ofn.hwndOwner = hWnd;
			switch (LOWORD(wParam)) {
				case ID_SEND_DOCUMENT:
					ofn.lpstrFilter = "TI-Nspire Documents (*.tns)\0*.tns\0"
					                  "All Files\0*.*\0";
					break;
				case ID_SEND_OS:
					if (emulate_cas)
						ofn.lpstrFilter = "TI-Nspire CAS OS images (*.tnc)\0*.tnc\0"
						                  "All Files\0*.*\0";
					else
						ofn.lpstrFilter = "TI-Nspire OS images (*.tno)\0*.tno\0"
						                  "All Files\0*.*\0";
					break;
				case ID_SEND_TI84_FILE:
					ofn.lpstrFilter = "TI-84+ Files (*.8xp, and so on)\0*.8x?;*.83?;*.82?\0"
					                  "All Files\0*.*\0";
					break;
			}
			ofn.lpstrFile = lParam.filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&ofn)) {
				DWORD result;
				strcpy(lParam.folder, target_folder);
				if (!SendMessageTimeout(hwndMessage, WM_USER, LOWORD(wParam), (LPARAM)&lParam,
					SMTO_NORMAL, 2000, &result))
				{
					MessageBox(hWnd, "SendMessage timeout", NULL, MB_OK);
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

DWORD CALLBACK gui_thread(LPVOID hEvent) {
	HANDLE hInstance = GetModuleHandle(NULL);
	WNDCLASS wc;
	MSG msg;
	RECT rc;
	HACCEL hAccel;

	memset(&wc, 0, sizeof wc);
	wc.lpfnWndProc = emu_wnd_proc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
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

	strcpy(target_folder, "Examples");

	rc.left = 0;
	rc.top = 0;
	rc.right = KEYPAD_WIDTH;
	rc.bottom = 240 + KEYPAD_HEIGHT;
	AdjustWindowRect(&rc, WS_VISIBLE | (WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX), TRUE);

	hwndMain = CreateWindow("nspire_emu", "nspire_emu",
		WS_VISIBLE | (WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, NULL, NULL);
	hMenu = GetMenu(hwndMain);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_ACCEL));

	hwndKeys = CreateWindow("nspire_keys", NULL, WS_VISIBLE | WS_CHILD,
		0, 240, KEYPAD_WIDTH, KEYPAD_HEIGHT, hwndMain, NULL, NULL, NULL);

	hwndGfx = CreateWindow("nspire_gfx", NULL, WS_VISIBLE | WS_CHILD,
		(KEYPAD_WIDTH - 320) / 2, 0, 320, 240, hwndMain, NULL, NULL, NULL);

	SetEvent(hEvent);

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
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hwndMessage = CreateWindow("Static", NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	SetWindowLong(hwndMessage, GWL_WNDPROC, (LONG)message_wnd_proc);

	CreateThread(NULL, 0, gui_thread, hEvent, 0, &gui_thread_id);
	WaitForSingleObject(hEvent, INFINITE);
	CloseHandle(hEvent);
}

void get_messages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
}


struct gui_saved_state {
	char target_folder[256];
};

void *gui_save_state(size_t *size) {
	*size = sizeof(struct gui_saved_state);
	struct gui_saved_state *state = malloc(*size);
	memcpy(&state->target_folder, target_folder, sizeof(target_folder));
	return state;
}

void gui_reload_state(void *state) {
	struct gui_saved_state *_state = (struct gui_saved_state *)state;
	memcpy(target_folder, &_state->target_folder, sizeof(target_folder));
}
