// Автор: Николай Фролов.

#include <MainWindow.h>

bool CMainWindow::RegisterClass()
{
    WNDCLASSEX windowWND;

    windowWND.cbSize = sizeof( WNDCLASSEX );
    windowWND.style = CS_HREDRAW | CS_VREDRAW;
    windowWND.lpfnWndProc = mainWindowProc;
    windowWND.cbClsExtra = 0;
    windowWND.cbWndExtra = 0;
    windowWND.hInstance = static_cast<HINSTANCE>( GetModuleHandle( 0 ) );
    windowWND.hIcon = 0;
    windowWND.hCursor = ::LoadCursor( 0, IDC_ARROW );
    windowWND.hbrBackground = 0;
    windowWND.lpszMenuName = MAKEINTRESOURCE( IDR_MENU1 );;
    windowWND.lpszClassName = L"CMainWindow";
    windowWND.hIconSm = 0;

    return ::RegisterClassEx( &windowWND ) != 0;
}

bool CMainWindow::Create()
{
    handle = ::CreateWindowEx( 0, L"CMainWindow", L"MemoryViewer", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VSCROLL 
		| ES_AUTOVSCROLL | WS_HSCROLL | ES_AUTOHSCROLL,	0, 0, 433, 600, 0, 0,
		static_cast<HINSTANCE>( ::GetModuleHandle( 0 ) ), this );

    return handle != 0;
}

void CMainWindow::Show( int cmdShow ) const
{
    ::ShowWindow( handle, cmdShow );
}

void CMainWindow::OnDestroy() const
{
    ::PostQuitMessage( 0 );
}

void CMainWindow::OnPaint() const
{
	drawer.DrawMemoryInfoInWindow( currentInfo, handle );
}

void CMainWindow::OnNewPID()
{
	// GetPIDFromUser();
	OnRefresh();
}

void CMainWindow::OnRefresh()
{
	if( currentProcessId != -1 ) {
		currentInfo = memoryInfoFinder.FindProcessInfo( currentProcessId );
		::InvalidateRect( handle, 0, true );
	}
}

LRESULT CMainWindow::wmCommandProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CMainWindow* window = reinterpret_cast<CMainWindow*>( ::GetWindowLong( hwnd, GWL_USERDATA ) );

	switch( HIWORD( wParam ) ) {
		case 0:
			switch( LOWORD( wParam ) ) {
				case ID_MENU_NEWPID:
					window->OnNewPID();
					break;
				case ID_MENU_REFRESH:
					window->OnRefresh();
					break;
				case ID_MENU_EXIT:
					window->OnDestroy();
					break;
				default:
					return ::DefWindowProc( hwnd, message, wParam, lParam );
			}
		default:
			return ::DefWindowProc( hwnd, message, wParam, lParam );
	}
	return 0;
}

LRESULT __stdcall CMainWindow::mainWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CMainWindow* window = reinterpret_cast<CMainWindow*>( ::GetWindowLong( hwnd, GWL_USERDATA ) );

    switch( message ) {
		case WM_NCCREATE:
			::SetWindowLong( hwnd, GWL_USERDATA, reinterpret_cast<LONG>( reinterpret_cast<CREATESTRUCT*>( lParam )->lpCreateParams ) );
			return ::DefWindowProc( hwnd, message, wParam, lParam );
			break;
		case WM_DESTROY:
			window->OnDestroy();
			break;
		case WM_HSCROLL:
			break;
		case WM_VSCROLL:
			break;
		case WM_COMMAND:
			return wmCommandProc( hwnd, message, wParam, lParam);
		case WM_PAINT:
			window->OnPaint();
			break;
		default:
			return ::DefWindowProc( hwnd, message, wParam, lParam );
    }
    return 0;
}