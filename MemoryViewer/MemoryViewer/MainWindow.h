// Автор: Николай Фролов.

// Описание: класс для главного окна приложения.

#pragma once

#include <ProcessInfoFinder.h>
#include <Drawer.h>
#include <resource.h>

#include <Windows.h>

class CMainWindow {
public:
	CMainWindow() : currentProcessId( -1 )
	{ }

	// Регистрация класса окна.
    static bool RegisterClass();

    // Создание экземпляра окна.
    bool Create();

    // Показать окно.
    void Show( int cmdShow ) const;

protected:
    void OnDestroy() const;

	void OnPaint() const;

	void OnNewPID();

	void OnRefresh();
private:
	// Описатель данного окна.
    HWND handle;

	// ID процесса, память которого отображается в данный момент.
	int currentProcessId;
	TProcessMemoryInfo currentInfo;
	// Механизм предоставляющий карту памяти процесса.
	static CProcessInfoFinder memoryInfoFinder;
	// Механизм отрисовки
	static CDrawer drawer;

	static LRESULT __stdcall wmCommandProc( HWND hanlde, UINT message, WPARAM wParam, LPARAM lParam );
	static LRESULT __stdcall mainWindowProc( HWND hanlde, UINT message, WPARAM wParam, LPARAM lParam );
};