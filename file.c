#include <windows.h>
#include <vector>



#include <algorithm>

void NewLine(void);
void ScrollScreenBuffer(HANDLE, INT);

HANDLE stdout, stdin;
CONSOLE_SCREEN_BUFFER_INFO info;


int main() {

	LPSTR prompt1 = "Type a line and press Enter, or q to quit: ";
	LPSTR prompt2 = "Type any key, or q to quit: ";
	CHAR buffer[256];
	DWORD read, written, mode, old_mode;
	WORD old_color_attrs;

	// Get handles to STDIN and STDOUT.
	stdin = GetStdHandle(STD_INPUT_HANDLE);
	stdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (stdin == INVALID_HANDLE_VALUE || stdout == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("GetStdHandle"), TEXT("Console Error"), MB_OK);
		return 1;
	}

	// Save the current text colors.
	if (!GetConsoleScreenBufferInfo(stdout, &info)) {
		MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"),
		TEXT("Console Error"), MB_OK);
		return 1;
	}

	old_color_attrs = info.wAttributes;

	// Set the text attributes to draw red text on black background.
	if (!SetConsoleTextAttribute(stdout, FOREGROUND_RED | FOREGROUND_INTENSITY)) {
		MessageBox(
			NULL, TEXT("SetConsoleTextAttribute"), TEXT("Console Error"), MB_OK);
		return 1;
	}

	// Write to STDOUT and read from STDIN by using the default
	// modes. Input is echoed automatically, and ReadFile
	// does not return until a carriage return is typed.
	//
	// The default input modes are line, processed, and echo.
	// The default output modes are processed and wrap at EOL.
	while (1) {
		// output handle, prompt string, string length, bytes written, not overlapped
		if (!WriteFile(stdout, prompt1, lstrlenA(prompt1), &written, NULL)) {
			MessageBox(NULL, TEXT("WriteFile"), TEXT("Console Error"), MB_OK);
			return 1;
		}

		// input handle, buffer to read into, size of buffer, actual bytes read, not overlapped
		if (!ReadFile(stdin, buffer, 255, &read, NULL))
			break;

		if (buffer[0] == 'q')
			break;
	}

	// Turn off the line input and echo input modes
	if (!GetConsoleMode(stdin, &old_mode)) {
		MessageBox(NULL, TEXT("GetConsoleMode"), TEXT("Console Error"), MB_OK);
		return 1;
	}

	mode = old_mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);

	if (!SetConsoleMode(stdin, mode)) {
		MessageBox(NULL, TEXT("SetConsoleMode"), TEXT("Console Error"),
		MB_OK);
		return 1;
	}

	// ReadFile returns when any input is available.
	// WriteFile is used to echo input.
	NewLine();

	while (1) {
		// output handle, prompt string, string length, bytes written, not overlapped
		if (!WriteFile(stdout, prompt2, lstrlenA(prompt2),  &written, NULL)) {
			MessageBox(NULL, TEXT("WriteFile"), TEXT("Console Error"),
			MB_OK);
			return 1;
		}

		if (!ReadFile(stdin, buffer, 1, &read, NULL))
			break;

		if (buffer[0] == '\r') {
			NewLine();
		} else if (! WriteFile(stdout, buffer, read, &written, NULL)) {
			break;
		} else {
			NewLine();
		}

		if (buffer[0] == 'q')
			break;
	}

	// Restore the original console mode.
	SetConsoleMode(stdin, old_mode);

	// Restore the original text colors.
	SetConsoleTextAttribute(stdout, old_color_attrs);

	return 0;
}

// The NewLine function handles carriage returns when the processed
// input mode is disabled. It gets the current cursor position
// and resets it to the first cell of the next row.
void NewLine(void) {
	if (!GetConsoleScreenBufferInfo(stdout, &info)) {
		MessageBox(NULL, TEXT("GetConsoleScreenBufferInfo"),
		TEXT("Console Error"), MB_OK);
		return;
	}

	info.dwCursorPosition.X = 0;

	// If it is the last line in the screen buffer, scroll
	// the buffer up.
	if ((info.dwSize.Y - 1) == info.dwCursorPosition.Y) {
		ScrollScreenBuffer(stdout, 1);
	} else {
		// Otherwise, advance the cursor to the next line.
		info.dwCursorPosition.Y += 1;
	}

	if (! SetConsoleCursorPosition(stdout, info.dwCursorPosition)) {
		MessageBox(NULL, TEXT("SetConsoleCursorPosition"),
		TEXT("Console Error"), MB_OK);
		return;
	}
}

void ScrollScreenBuffer(HANDLE h, INT x) {
	SMALL_RECT srctScrollRect, srctClipRect;
	CHAR_INFO chiFill;
	COORD coordDest;
	srctScrollRect.Left = 0;
	srctScrollRect.Top = 1;
	srctScrollRect.Right = info.dwSize.X - (SHORT)x;
	srctScrollRect.Bottom = info.dwSize.Y - (SHORT)x;

	// The destination for the scroll rectangle is one row up.
	coordDest.X = 0;
	coordDest.Y = 0;

	// The clipping rectangle is the same as the scrolling rectangle.
	// The destination row is left unchanged.
	srctClipRect = srctScrollRect;

	// Set the fill character and attributes.
	chiFill.Attributes = FOREGROUND_RED|FOREGROUND_INTENSITY;
	chiFill.Char.AsciiChar = (char)' ';

	// Scroll up one line.
	// screen buffer handle, scrolling rectangle, clipping rectangle, 
	// top left destination cell, fill character and color
	ScrollConsoleScreenBuffer(
		h, &srctScrollRect, &srctClipRect, coordDest, &chiFill);
}
