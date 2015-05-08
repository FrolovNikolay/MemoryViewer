// Автор: Николай Фролов.

// Описание: класс отрисовки главного окна приложения. Not-Thread-Safe

#pragma once

#include <ProcessInfoFinder.h>

#include <Windows.h>

class CDrawer {
public:
	static void DrawMemoryInfoInWindow( const TProcessMemoryInfo& memoryInfo, HWND window );

private:
	static const int fieldHeight = 40;
	static const int fieldWidth = 80;

	static void drawHeader( HDC hdc );
	static void drawRegion( HDC hdc, const CRegionInfo& regionInfo );
	static void drawBlock( HDC hdc, const CBlockInfo& blockInfo );

	static void drawNextFieldInRow( HDC hdc, const wchar_t* str );

	// Данные по отрисовке, которые необходимо передавать междуфункциями.
	static int currentX;
	static int currentY;
};