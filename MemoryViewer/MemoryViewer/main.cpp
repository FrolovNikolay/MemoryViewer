// Автор: Николай Фролов.

#include <ProcessInfoFinder.h>
#include <MainWindow.h>

#include <fstream>

int _stdcall wWinMain( HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR commandLine, int nCmdShow )
{
	CMainWindow::RegisterClass();
    CMainWindow mainWindow;
    mainWindow.Create();
    mainWindow.Show( nCmdShow );
    MSG msg;
    while( ::GetMessage( &msg, 0, 0, 0 ) != 0 ) {
        ::TranslateMessage( &msg );
        ::DispatchMessage( &msg );
    }
	return 0;
}