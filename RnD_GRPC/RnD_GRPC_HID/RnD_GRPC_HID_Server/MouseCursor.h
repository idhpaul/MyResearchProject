#include "CursorStruct.h"

#include <Windows.h>

#include <iostream>
#include <memory>



class MouseCursor
{
public:
	MouseCursor();
	~MouseCursor();

	CursorPayload CaptureCursor();

private:
	bool isSameCursorShape(const CURSORINFO& nowCursorInfo, const CURSORINFO& prevCursorInfo);
	bool mouseCursorFromHCursor(HDC& dc, HCURSOR& cursor, CursorPayload& payloadData);

private:
	HDC ScreenDC_ = nullptr;
	HDC hDC_ = nullptr;

	static CURSORINFO prevCursorInfo_;

};

