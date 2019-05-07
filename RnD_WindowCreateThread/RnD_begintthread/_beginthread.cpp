// crt_BEGTHRD.C
// compile with: /MT /D "_X86_" /c
// processor: x86
#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */
#include <stddef.h>
#include <stdlib.h>
#include <conio.h>

void Bounce(void *);
void CheckKey(void *);

// GetRandom returns a random integer between min and max.
#define GetRandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))
// GetGlyph returns a printable ASCII character value
#define GetGlyph( val ) ((char)((val + 32) % 93 + 33))

BOOL repeat = TRUE;                 // Global repeat flag
HANDLE hStdOut;                     // Handle for console window
CONSOLE_SCREEN_BUFFER_INFO csbi;    // Console information structure

int main()
{
	int param = 0;
	int * pparam = &param;

	// Get display screen's text row and column information.
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOut, &csbi);

	// Launch CheckKey thread to check for terminating keystroke.
	_beginthread(CheckKey, 0, NULL);

	// Loop until CheckKey terminates program or 1000 threads created.
	while (repeat && param < 1000)
	{
		// launch another character thread.
		_beginthread(Bounce, 0, (void *)pparam);

		// increment the thread parameter
		param++;

		// Wait one second between loops.
		Sleep(1000L);
	}
}

// CheckKey - Thread to wait for a keystroke, then clear repeat flag.
void CheckKey(void * ignored)
{
	_getch();
	repeat = 0;    // _endthread implied
}

// Bounce - Thread to create and control a colored letter that moves
// around on the screen.
//
// Params: parg - the value to create the character from
void Bounce(void * parg)
{
	char       blankcell = 0x20;
	CHAR_INFO  ci;
	COORD      oldcoord, cellsize, origin;
	DWORD      result;
	SMALL_RECT region;

	cellsize.X = cellsize.Y = 1;
	origin.X = origin.Y = 0;

	// Generate location, letter and color attribute from thread argument.
	srand(_threadid);
	oldcoord.X = region.Left = region.Right =
		GetRandom(csbi.srWindow.Left, csbi.srWindow.Right - 1);
	oldcoord.Y = region.Top = region.Bottom =
		GetRandom(csbi.srWindow.Top, csbi.srWindow.Bottom - 1);
	ci.Char.AsciiChar = GetGlyph(*((int *)parg));
	ci.Attributes = GetRandom(1, 15);

	while (repeat)
	{
		// Pause between loops.
		Sleep(100L);

		// Blank out our old position on the screen, and draw new letter.
		WriteConsoleOutputCharacterA(hStdOut, &blankcell, 1, oldcoord, &result);
		WriteConsoleOutputA(hStdOut, &ci, cellsize, origin, &region);

		// Increment the coordinate for next placement of the block.
		oldcoord.X = region.Left;
		oldcoord.Y = region.Top;
		region.Left = region.Right += GetRandom(-1, 1);
		region.Top = region.Bottom += GetRandom(-1, 1);

		// Correct placement (and beep) if about to go off the screen.
		if (region.Left < csbi.srWindow.Left)
			region.Left = region.Right = csbi.srWindow.Left + 1;
		else if (region.Right >= csbi.srWindow.Right)
			region.Left = region.Right = csbi.srWindow.Right - 2;
		else if (region.Top < csbi.srWindow.Top)
			region.Top = region.Bottom = csbi.srWindow.Top + 1;
		else if (region.Bottom >= csbi.srWindow.Bottom)
			region.Top = region.Bottom = csbi.srWindow.Bottom - 2;

		// If not at a screen border, continue, otherwise beep.
		else
			continue;
		Beep((ci.Char.AsciiChar - 'A') * 100, 175);
	}
	// _endthread given to terminate
	_endthread();
}