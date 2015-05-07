// Автор: Николай Фролов.

// Описание: класс отрисовки главного окна приложения

#pragma once

#include <ProcessInfoFinder.h>

#include <Windows.h>

class CDrawer {
public:
	static void DrawMemoryInfoInWindow( const TProcessMemoryInfo& memoryInfo, HWND window );
};