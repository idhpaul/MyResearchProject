#pragma once
#include "CursorStruct.h"
#include <Windows.h>


void mouseCursorFromHCursor(HDC dc, HCURSOR cursor, struct CursorPayload* cursordata);
void CursorInfo(struct CursorPayload* cursordata);