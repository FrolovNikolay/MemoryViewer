// Автор: Николай Фролов.

#include <MainWindow.h>
#include <string>
#include <memory>

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
    handle = ::CreateWindowEx( 0, L"CMainWindow", L"MemoryViewer", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VSCROLL, 
			 0, 0, width, height, 0, 0, static_cast<HINSTANCE>( ::GetModuleHandle( 0 ) ), this );

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
	drawer.DrawMemoryInfoInWindow( currentInfo, handle, verticalScrollUnit );
}

void CMainWindow::OnNewPID()
{
	getPIDFromUser();
}

void CMainWindow::OnRefresh()
{
	if( currentProcessId != -1 ) {
		if( currentProcessId < 0  || currentProcessId > 65536 ) {
			::MessageBox( handle, L"Некорректно указан PID. Ожидается значение из отрезка [0, 65536]",  L"Ошибка!", MB_ICONERROR | MB_OK );
			currentInfo = TProcessMemoryInfo();
			currentProcessId = -1;
		} else {
			currentInfo = memoryInfoFinder.FindProcessInfo( currentProcessId );
			if( currentInfo.empty() ) {
				::MessageBox( handle, L"Приложение с указанным PID не доступно или не существует.", L"Ошибка!", MB_ICONERROR | MB_OK );
				currentProcessId = -1;
			}
		}
		::SetScrollPos( handle, SB_VERT, 0, 0 );
		calculateCurrentScrollUnit();
		::InvalidateRect( handle, 0, true );
	}
}

void CMainWindow::OnVScroll( WPARAM wParam, LPARAM lParam ) const
{
	if( lParam == 0 ) { 
		SCROLLINFO scrollInfo; 
		scrollInfo.cbSize = sizeof( SCROLLINFO ); 
		scrollInfo.fMask = SIF_ALL; 
		// Меняем позицию скролла 
		::GetScrollInfo( handle, SB_VERT, &scrollInfo ); 
 		switch( LOWORD( wParam ) ) { 
 		case SB_LINEUP: 
 			scrollInfo.nPos = max( scrollInfo.nPos - 1, scrollInfo.nMin ); 
 			break; 
 		case SB_LINEDOWN: 
 			scrollInfo.nPos = min( scrollInfo.nPos + 1, scrollInfo.nMax ); 
 			break; 
 		case SB_PAGEUP: 
 			scrollInfo.nPos = max( scrollInfo.nPos - static_cast<int>( scrollInfo.nPage ), scrollInfo.nMin ); 
 			break; 
 		case SB_PAGEDOWN: 
 			scrollInfo.nPos = min( scrollInfo.nPos + static_cast<int>( scrollInfo.nPage ), scrollInfo.nMax ); 
 			break; 
 		case SB_THUMBTRACK: 
 			scrollInfo.nPos = scrollInfo.nTrackPos; 
 			break; 
 		} 
 		// Устанавливаем новую позицию скролла 
 		int scrollPosition = scrollInfo.nPos; 
 		scrollInfo.fMask = SIF_POS; 
 		::RedrawWindow( handle,  0, 0, RDW_INVALIDATE ); 
 		::SetScrollInfo( handle, SB_VERT, &scrollInfo, TRUE ); 
	}
}

void CMainWindow::calculateCurrentScrollUnit() const
{
	verticalScrollUnit = 0;
	int minScrollPosition = 0;
	int maxScrollPosition = 0;
	::GetScrollRange( handle, SB_VERT, &minScrollPosition, &maxScrollPosition );

	int totalHeight = 0;
	for( auto& region : currentInfo ) {
		totalHeight += drawer.GetFieldHeight();
		for( auto& block : region.Blocks ) {
			totalHeight += drawer.GetFieldHeight();
		}
	}
	verticalScrollUnit = totalHeight / ( maxScrollPosition - minScrollPosition );
}

void CMainWindow::getPIDFromUser()
{
	if( ::DialogBox( static_cast<HINSTANCE>( ::GetModuleHandle( 0 ) ), MAKEINTRESOURCE( IDD_DIALOG1 ),
		handle, lpDialogProc ) == IDOK ) {
		OnRefresh();
	}
}

BOOL CALLBACK CMainWindow::lpDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HWND parent = ::GetParent( hwnd );
	switch( msg ) {
		case WM_INITDIALOG:
			break;
		case WM_COMMAND:
			switch( LOWORD( wParam ) ) {
				case IDOK:
				{
					HWND text = ::GetDlgItem( hwnd, IDC_EDIT1 );
					int textSize = ::GetWindowTextLength( text );
					std::unique_ptr<wchar_t> PID( new wchar_t[textSize + 1] );
					::GetWindowText( text, PID.get(), textSize + 1 );
					int intPID = 0;
					try { 
						 intPID = std::stoi( std::wstring( PID.get() ) );
					} catch( ... ) {
						// Введен некорректный PID, для сообщения пользователю делаем его невалидным.
						intPID = -2;
					}
					CMainWindow* window = reinterpret_cast<CMainWindow*>( ::GetWindowLong( parent, GWL_USERDATA ) );
					window->currentProcessId = intPID;
					return ::EndDialog( hwnd, true );
				}
				case IDCANCEL:
					return ::EndDialog( hwnd, false );
			}
	}
	return 0;
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
		case WM_VSCROLL:
			window->OnVScroll( wParam, lParam );
			break;
		case WM_COMMAND:
			return wmCommandProc( hwnd, message, wParam, lParam);
		case WM_PAINT:
			window->OnPaint();
			return ::DefWindowProc( hwnd, message, wParam, lParam );
		default:
			return ::DefWindowProc( hwnd, message, wParam, lParam );
    }
    return 0;
}