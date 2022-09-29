#ifndef SINGLEFILE_WAITKEY_H
#define SINGLEFILE_WAITKEY_H

#if defined(__cplusplus)
extern "C" {
#endif

#define WK_IMPLEMENTATION

#if defined(WK_DEBUG)
#include <stdio.h>
#endif

enum WK_NUM {
	// ASCII keys
    WKK_NONE = 0x00,
    WKK_TAB = 0x09,
    WKK_NEWLINE,
    WKK_RETURN = 0x0D,
    WKK_ESCAPE = 0x1B,
    WKK_SPACE = 0x20,
    WKK_EXCLAMATION,
    WKK_DOUBLE_QUOTE,
    WKK_HASH,
    WKK_DOLLAR,
    WKK_PERCENT,
    WKK_AMPERSAND,
    WKK_SINGLE_QUOTE,
    WKK_LEFT_PAREN,
    WKK_RIGHT_PAREN,
    WKK_ASTERISK,
    WKK_PLUS,
    WKK_COMMA,
    WKK_MINUS,
    WKK_PERIOD,
    WKK_SLASH,
    WKK_0,
    WKK_1,
    WKK_2,
    WKK_3,
    WKK_4,
    WKK_5,
    WKK_6,
    WKK_7,
    WKK_8,
    WKK_9,
    WKK_COLON,
    WKK_SEMICOLON,
    WKK_LESS_THAN,
    WKK_EQUALS,
    WKK_GREATER_THAN,
    WKK_QUESTION,
    WKK_AT,
    WKK_CAP_A,
    WKK_CAP_B,
    WKK_CAP_C,
    WKK_CAP_D,
    WKK_CAP_E,
    WKK_CAP_F,
    WKK_CAP_G,
    WKK_CAP_H,
    WKK_CAP_I,
    WKK_CAP_J,
    WKK_CAP_K,
    WKK_CAP_L,
    WKK_CAP_M,
    WKK_CAP_N,
    WKK_CAP_O,
    WKK_CAP_P,
    WKK_CAP_Q,
    WKK_CAP_R,
    WKK_CAP_S,
    WKK_CAP_T,
    WKK_CAP_U,
    WKK_CAP_V,
    WKK_CAP_W,
    WKK_CAP_X,
    WKK_CAP_Y,
    WKK_CAP_Z,
    WKK_LEFT_BRACKET,
    WKK_BACKSLASH,
    WKK_RIGHT_BRACKET,
    WKK_CARET,
    WKK_UNDERSCORE,
    WKK_GRAVE,
    WKK_A,
    WKK_B,
    WKK_C,
    WKK_D,
    WKK_E,
    WKK_F,
    WKK_G,
    WKK_H,
    WKK_I,
    WKK_J,
    WKK_K,
    WKK_L,
    WKK_M,
    WKK_N,
    WKK_O,
    WKK_P,
    WKK_Q,
    WKK_R,
    WKK_S,
    WKK_T,
    WKK_U,
    WKK_V,
    WKK_W,
    WKK_X,
    WKK_Y,
    WKK_Z,
    WKK_LEFT_CURLY,
    WKK_PIPE,
    WKK_RIGHT_CURLY,
    WKK_TILDE,
    WKK_BACKSPACE = 0x7F,

    // special keys
    WKK_UP = 0x1000,
    WKK_DOWN,
    WKK_RIGHT,
    WKK_LEFT,
    WKK_DELETE,
    WKK_HOME,
    WKK_END,
    WKK_PGUP,
    WKK_PGDN,
    WKK_INS,
};

typedef enum {
	WKC_BLACK = 0,
    WKC_RED,
    WKC_GREEN,
    WKC_YELLOW,
    WKC_BLUE,
    WKC_MAGENTA,
    WKC_CYAN,
    WKC_WHITE,
    WKC_KEEP,
    WKC_DEFAULT
} wk_color_t;

#if !defined(WK_NO_NAMES)
const char *wk_get_key_names(int key);
#endif

int wk_waitkey(void);
const char *wk_get_terminal(void);
void wk_get_screen_size(int *rows, int *cols);
void wk_set_color(int foreground, int background);

#define wk_reset_color()		wk_set_color(WKC_DEFAULT, WKC_DEFAULT)

#if defined(WK_IMPLEMENTATION)

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#include <windows.h>
#include <conio.h>
#define WK_WINDOWS
#else
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <langinfo.h>
#include <sys/ioctl.h>
#endif

#include <stdlib.h>
#include <string.h>

static int WK_BEGIN = 0;
static int WK_COUNT = 0;
static char WK_BUFFER[8];

static int wk_read_input(void)
{
	if (WK_COUNT > 0) {
		return 0;
	}

	WK_BEGIN = WK_COUNT = 0;

#if defined(WK_WINDOWS)
	WK_BUFFER[WK_COUNT++] = _getch();
	while (_kbhit() && (WK_COUNT < sizeof(WK_BUFFER))) {
		WK_BUFFER[WK_COUNT++] = _getch();
	}
#else
	struct termios t;
	size_t pending = 0;
	ssize_t result = 0;
	tcflag_t tc_previous = 0;

	tcgetattr(STDIN_FILENO, &t);
	tc_previous = t.c_lflag;
	t.c_lflag = t.c_lflag & (tcflag_t)(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

	result = read(STDIN_FILENO, WK_BUFFER, 1);
	if (result == 1) {
		WK_COUNT = 1;
		ioctl(STDIN_FILENO, FIONREAD, &pending);
		if (pending > (sizeof(WK_BUFFER) - (size_t)WK_COUNT)) {
			pending = sizeof(WK_BUFFER) - (size_t)WK_COUNT;
		}

		if (pending) {
			result = read(STDIN_FILENO, WK_BUFFER + WK_COUNT, pending);
			if (result == (ssize_t)pending) {
				WK_COUNT += (int)pending;
			}
		}
	}

	t.c_lflag = tc_previous;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif

	return WK_COUNT;
}

#if !defined(WK_NO_NAMES)
const char *wk_get_key_names(int key)
{
	switch (key) {
		case WKK_TAB:           return "WKK_TAB";
        case WKK_NEWLINE:       return "WKK_NEWLINE";
        case WKK_RETURN:        return "WKK_RETURN";
        case WKK_ESCAPE:        return "WKK_ESCAPE";
        case WKK_SPACE:         return "WKK_SPACE";
        case WKK_EXCLAMATION:   return "WKK_EXCLAMATION";
        case WKK_DOUBLE_QUOTE:  return "WKK_DOUBLE_QUOTE";
        case WKK_HASH:          return "WKK_HASH";
        case WKK_DOLLAR:        return "WKK_DOLLAR";
        case WKK_PERCENT:       return "WKK_PERCENT";
        case WKK_AMPERSAND:     return "WKK_AMPERSAND";
        case WKK_SINGLE_QUOTE:  return "WKK_SINGLE_QUOTE";
        case WKK_LEFT_PAREN:    return "WKK_LEFT_PAREN";
        case WKK_RIGHT_PAREN:   return "WKK_RIGHT_PAREN";
        case WKK_ASTERISK:      return "WKK_ASTERISK";
        case WKK_PLUS:          return "WKK_PLUS";
        case WKK_COMMA:         return "WKK_COMMA";
        case WKK_MINUS:         return "WKK_MINUS";
        case WKK_PERIOD:        return "WKK_PERIOD";
        case WKK_SLASH:         return "WKK_SLASH";
        case WKK_0:             return "WKK_0";
        case WKK_1:             return "WKK_1";
        case WKK_2:             return "WKK_2";
        case WKK_3:             return "WKK_3";
        case WKK_4:             return "WKK_4";
        case WKK_5:             return "WKK_5";
        case WKK_6:             return "WKK_6";
        case WKK_7:             return "WKK_7";
        case WKK_8:             return "WKK_8";
        case WKK_9:             return "WKK_9";
        case WKK_COLON:         return "WKK_COLON";
        case WKK_SEMICOLON:     return "WKK_SEMICOLON";
        case WKK_LESS_THAN:     return "WKK_LESS_THAN";
        case WKK_EQUALS:        return "WKK_EQUALS";
        case WKK_GREATER_THAN:  return "WKK_GREATER_THAN";
        case WKK_QUESTION:      return "WKK_QUESTION";
        case WKK_AT:            return "WKK_AT";
        case WKK_CAP_A:         return "WKK_CAP_A";
        case WKK_CAP_B:         return "WKK_CAP_B";
        case WKK_CAP_C:         return "WKK_CAP_C";
        case WKK_CAP_D:         return "WKK_CAP_D";
        case WKK_CAP_E:         return "WKK_CAP_E";
        case WKK_CAP_F:         return "WKK_CAP_F";
        case WKK_CAP_G:         return "WKK_CAP_G";
        case WKK_CAP_H:         return "WKK_CAP_H";
        case WKK_CAP_I:         return "WKK_CAP_I";
        case WKK_CAP_J:         return "WKK_CAP_J";
        case WKK_CAP_K:         return "WKK_CAP_K";
        case WKK_CAP_L:         return "WKK_CAP_L";
        case WKK_CAP_M:         return "WKK_CAP_M";
        case WKK_CAP_N:         return "WKK_CAP_N";
        case WKK_CAP_O:         return "WKK_CAP_O";
        case WKK_CAP_P:         return "WKK_CAP_P";
        case WKK_CAP_Q:         return "WKK_CAP_Q";
        case WKK_CAP_R:         return "WKK_CAP_R";
        case WKK_CAP_S:         return "WKK_CAP_S";
        case WKK_CAP_T:         return "WKK_CAP_T";
        case WKK_CAP_U:         return "WKK_CAP_U";
        case WKK_CAP_V:         return "WKK_CAP_V";
        case WKK_CAP_W:         return "WKK_CAP_W";
        case WKK_CAP_X:         return "WKK_CAP_X";
        case WKK_CAP_Y:         return "WKK_CAP_Y";
        case WKK_CAP_Z:         return "WKK_CAP_Z";
        case WKK_LEFT_BRACKET:  return "WKK_LEFT_BRACKET";
        case WKK_BACKSLASH:     return "WKK_BACKSLASH";
        case WKK_RIGHT_BRACKET: return "WKK_RIGHT_BRACKET";
        case WKK_CARET:         return "WKK_CARET";
        case WKK_UNDERSCORE:    return "WKK_UNDERSCORE";
        case WKK_GRAVE:         return "WKK_GRAVE";
        case WKK_A:             return "WKK_A";
        case WKK_B:             return "WKK_B";
        case WKK_C:             return "WKK_C";
        case WKK_D:             return "WKK_D";
        case WKK_E:             return "WKK_E";
        case WKK_F:             return "WKK_F";
        case WKK_G:             return "WKK_G";
        case WKK_H:             return "WKK_H";
        case WKK_I:             return "WKK_I";
        case WKK_J:             return "WKK_J";
        case WKK_K:             return "WKK_K";
        case WKK_L:             return "WKK_L";
        case WKK_M:             return "WKK_M";
        case WKK_N:             return "WKK_N";
        case WKK_O:             return "WKK_O";
        case WKK_P:             return "WKK_P";
        case WKK_Q:             return "WKK_Q";
        case WKK_R:             return "WKK_R";
        case WKK_S:             return "WKK_S";
        case WKK_T:             return "WKK_T";
        case WKK_U:             return "WKK_U";
        case WKK_V:             return "WKK_V";
        case WKK_W:             return "WKK_W";
        case WKK_X:             return "WKK_X";
        case WKK_Y:             return "WKK_Y";
        case WKK_Z:             return "WKK_Z";
        case WKK_LEFT_CURLY:    return "WKK_LEFT_CURLY";
        case WKK_PIPE:          return "WKK_PIPE";
        case WKK_RIGHT_CURLY:   return "WKK_RIGHT_CURLY";
        case WKK_TILDE:         return "WKK_TILDE";
        case WKK_BACKSPACE:     return "WKK_BACKSPACE";
        case WKK_UP:            return "WKK_UP";
        case WKK_DOWN:          return "WKK_DOWN";
        case WKK_RIGHT:         return "WKK_RIGHT";
        case WKK_LEFT:          return "WKK_LEFT";
        case WKK_DELETE:        return "WKK_DELETE";
        case WKK_HOME:          return "WKK_HOME";
        case WKK_END:           return "WKK_END";
        case WKK_PGUP:          return "WKK_PGUP";
        case WKK_PGDN:          return "WKK_PGDN";
        case WKK_INS:           return "WKK_INS";
        default:                return "WKK_NONE";
	}
}
#endif

static int wk_interpret(const char value)
{
	if (value != WKK_ESCAPE) {
		if ((value < WKK_TAB) || ((value > WKK_RETURN) && (value < WKK_ESCAPE)) || ((value > WKK_ESCAPE) && (value < WKK_SPACE))) {
			return WKK_NONE;
		}
	}

	return (int)value;
}

static int wk_read(int deviceRead)
{
	if ((WK_COUNT != 0) || (deviceRead && (wk_read_input() > 0))) {
		WK_COUNT--;
#if defined(WK_DEBUG)
		printf("%d\n", WK_BUFFER[WK_BEGIN]);
#endif
		return wk_interpret(WK_BUFFER[WK_BEGIN++]);
	}

	return WKK_NONE;
}

#define reset_and_return(x)		\
	do {						\
		WK_COUNT = 0;			\
		return (x);				\
	} while (0)

static const char *WKT_XTERM = "xterm";
static const char *WKT_LINUX = "linux";
static const char *WKT_WINDOWS = "windows";

int wk_waitkey(void)
{
	if (wk_get_terminal() == WKT_WINDOWS) {
		int input = wk_read(1);
		if (input == 0) {
			int input = wk_read(0);
			if (input == 77) {
				reset_and_return(WKK_LEFT);
			} else if (input == 75) {
				reset_and_return(WKK_RIGHT);
			} else if (input == 73) {
				reset_and_return(WKK_PGUP);
			} else if (input == 72) {
				reset_and_return(WKK_UP);
			} else if (input == 71) {
				reset_and_return(WKK_HOME);
			} else if (input == 82) {
				reset_and_return(WKK_INS);
			} else if (input == 83) {
				reset_and_return(WKK_DELETE);
			} else if (input == 80) {
				reset_and_return(WKK_DOWN);
			} else if (input == 81) {
				reset_and_return(WKK_PGDN);
			} else if (input == 79) {
				reset_and_return(WKK_END);
			}

			reset_and_return(WKK_NONE);
		} else if (input == 224) {
			int input = wk_read(0);
			if (input == 77) {
				reset_and_return(WKK_LEFT);
			} else if (input == 75) {
				reset_and_return(WKK_RIGHT);
			} else if (input == 73) {
				reset_and_return(WKK_PGUP);
			} else if (input == 72) {
				reset_and_return(WKK_UP);
			} else if (input == 71) {
				reset_and_return(WKK_HOME);
			} else if (input == 82) {
				reset_and_return(WKK_INS);
			} else if (input == 83) {
				reset_and_return(WKK_DELETE);
			} else if (input == 80) {
				reset_and_return(WKK_DOWN);
			} else if (input == 81) {
				reset_and_return(WKK_PGDN);
			} else if (input == 79) {
				reset_and_return(WKK_END);
			}

			reset_and_return(WKK_NONE);
		}

		reset_and_return(input);
	}

	if (wk_get_terminal() == WKT_XTERM) {
		int input = wk_read(1);
		if (input == WKK_ESCAPE) {
			int input = wk_read(0);
			if (input == WKK_LEFT_BRACKET) {
				int input = wk_read(0);
				if (input == WKK_CAP_H) {
					reset_and_return(WKK_HOME);
				} else if (input == WKK_3) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_DELETE);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_2) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_INS);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_5) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_PGUP);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_6) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_PGDN);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_CAP_A) {
					reset_and_return(WKK_UP);
				} else if (input == WKK_CAP_B) {
					reset_and_return(WKK_DOWN);
				} else if (input == WKK_CAP_C) {
					reset_and_return(WKK_RIGHT);
				} else if (input == WKK_CAP_D) {
					reset_and_return(WKK_LEFT);
				} else if (input == WKK_CAP_F) {
					reset_and_return(WKK_END);
				}

				reset_and_return(WKK_NONE);
			}

			reset_and_return(WKK_NONE);
		}

		reset_and_return(input);
	}

	if (wk_get_terminal() == WKT_LINUX) {
		int input = wk_read(1);
		if (input == WKK_ESCAPE) {
			int input = wk_read(0);
			if (input == WKK_LEFT_BRACKET) {
				int input = wk_read(0);
				if (input == WKK_1) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_HOME);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_3) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_DELETE);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_2) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_INS);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_5) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_PGUP);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_4) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_END);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_6) {
					int input = wk_read(0);
					if (input == WKK_TILDE) {
						reset_and_return(WKK_PGDN);
					}

					reset_and_return(WKK_NONE);
				} else if (input == WKK_CAP_A) {
					reset_and_return(WKK_UP);
				} else if (input == WKK_CAP_B) {
					reset_and_return(WKK_DOWN);
				} else if (input == WKK_CAP_C) {
					reset_and_return(WKK_RIGHT);
				} else if (input == WKK_CAP_D) {
					reset_and_return(WKK_LEFT);
				}

				reset_and_return(WKK_NONE);
			}

			reset_and_return(WKK_NONE);
		}

		reset_and_return(input);
	}

	reset_and_return(WKK_NONE);
}

const char *wk_get_terminal(void)
{
	static const char *wk_currenTerm = NULL;
	
#if defined(WK_WINDOWS)
	return wk_currenTerm = WKT_WINDOWS;
#else
	if (wk_currenTerm != NULL) {
		return wk_currenTerm;
	}

	wk_currenTerm = getenv("TERM");
	if (wk_currenTerm == NULL) {
		wk_currenTerm = WKT_XTERM;
	}

	if (strcmp(wk_currenTerm, "windows") == 0) {
		return (wk_currenTerm = WKT_WINDOWS);
	}

	if (strcmp(wk_currenTerm, "xterm") == 0) {
		return (wk_currenTerm = WKT_XTERM);
	}

	if (strcmp(wk_currenTerm, "linux") == 0) {
		return (wk_currenTerm = WKT_LINUX);
	}

	return wk_currenTerm = WKT_XTERM;
#endif
}
#undef reset_and_return

void wk_get_screen_size(int *rows, int *cols)
{
#if defined(WK_WINDOWS)
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFOEX info;

	if (GetConsoleScreenBufferInfoEx(handle, &info) == TRUE) {
		if (rows) {
			*rows = info.srWindow.Bottom - info.swWindow.Top;
		}

		if (cols) {
			*cols = info.srWindow.Right - info.swWindow.Left;
		}
	}
#else
	struct winsize sz;

	if (ioctl(0, TIOCGWINSZ, &sz) == 0) {
		if (rows) {
			*rows = sz.ws_row;
		}

		if (cols) {
			*cols = sz.ws_col;
		}
	}
#endif
}

void wk_set_color(int foreground, int background)
{
	if ((foreground < 0) || (foreground > 9)) {
		return;
	}

	if ((background < 0) || (background > 9)) {
		return;
	}

#if defined(WK_WINDOWS)

#else
	if (!isatty(STDOUT_FILENO)) {
		return;
	}

	if (foreground != WKC_KEEP) {
		printf("\033[3%dm", foreground);
	}

	if (background != WKC_KEEP) {
		printf("\033[4%dm", background);
	}
#endif
}
#endif

#if defined(__cplusplus)
}
#endif

#endif
