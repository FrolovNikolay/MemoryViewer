// Автор: Николай Фролов.

#include <Drawer.h>

#include <string>
#include <cassert>
#include <sstream>

int CDrawer::currentX = 0;

int CDrawer::currentY = 0;

void CDrawer::DrawMemoryInfoInWindow( const TProcessMemoryInfo& memoryInfo, HWND window, int ScrollUnit )
{
	if( memoryInfo.empty() ) {
		return;
	}
	PAINTSTRUCT paintInfo;
	HDC displayHandle = ::BeginPaint( window, &paintInfo );
	assert( displayHandle != 0 );

	RECT rectInfo = paintInfo.rcPaint;
	int width = rectInfo.right;
	int height = rectInfo.bottom;

	HDC tempHDC = ::CreateCompatibleDC( displayHandle );
	HBITMAP tempBitmap = ::CreateCompatibleBitmap( displayHandle, width, height );
	HBITMAP oldBitmap = static_cast<HBITMAP>( ::SelectObject( tempHDC, tempBitmap ) );
	::Rectangle( tempHDC, 0, 0, width, height );

	SCROLLINFO scrollInfo; 
 	::ZeroMemory( &scrollInfo, sizeof( SCROLLINFO ) ); 
 	scrollInfo.cbSize = sizeof( SCROLLINFO ); 
 	scrollInfo.fMask = SIF_ALL; 
 	::GetScrollInfo( window, SB_VERT, &scrollInfo ); 
 	currentY = - scrollInfo.nPos * ScrollUnit; 

	currentX = 0;
	drawHeader( tempHDC );
	for( auto& region : memoryInfo ) {
		drawRegion( tempHDC, region );
	}
	
	::BitBlt( displayHandle, 0, 0, width, height, tempHDC, 0, 0, SRCCOPY );

	::DeleteObject( oldBitmap );
	::DeleteObject( tempBitmap );
	::DeleteObject( tempHDC );
	::EndPaint( window, &paintInfo );
}

void CDrawer::drawHeader( HDC hdc )
{
	wchar_t* header = L"Type";
	drawNextFieldInRow( hdc, header );

	header = L"Start";
	drawNextFieldInRow( hdc, header );

	header = L"Size";
	drawNextFieldInRow( hdc, header );

	header = L"Protection";
	drawNextFieldInRow( hdc, header );

	header = L"hasStack";
	drawNextFieldInRow( hdc, header );

	currentY += fieldHeight;
}

void CDrawer::drawNextFieldInRow( HDC hdc, const wchar_t* str )
{
	RECT currField;
	currField.left = currentX;
	currField.top = currentY;
	currField.right = ( currentX += fieldWidth );
	currField.bottom = currentY + fieldHeight;

	::Rectangle( hdc, currField.left, currField.top, currField.right, currField.bottom );
	::DrawText( hdc, str, -1, &currField, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
}

void CDrawer::drawRegion( HDC hdc, const CRegionInfo& regionInfo )
{
	currentX = 0;
	drawNextFieldInRow( hdc, L"region" );
	std::wstringstream str;

	str << std::hex << regionInfo.RegionBaseAddress;
	std::wstring header( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	str.str( std::wstring() );
	str.clear();
	str << std::dec << regionInfo.RegionSize;
	header = std::wstring( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	str.str( std::wstring() );
	str.clear();
	str << std::hex << protectionConverter( regionInfo.RegionProtection );
	header = std::wstring( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	str.str( std::wstring() );
	str.clear();
	str << ( regionInfo.NumberOfGuardedBlocks > 0 ? L"yes" : L"no" );
	header = std::wstring( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	currentY += fieldHeight;

	for( auto& block : regionInfo.Blocks ) {
		drawBlock( hdc, block );
	}
}

void CDrawer::drawBlock( HDC hdc, const CBlockInfo& blockInfo )
{
	currentX = 0;
	::MoveToEx( hdc, currentX + 10, currentY, 0 );
	::LineTo( hdc, currentX + 10, currentY + fieldHeight );
	::MoveToEx( hdc, currentX + 10, currentY + fieldHeight / 2, 0 );
	::LineTo( hdc, currentX + 20, currentY + fieldHeight / 2 );
	currentX += 20;
	drawNextFieldInRow( hdc, L"block" );
	std::wstringstream str;

	str << std::hex << blockInfo.BlockBaseAddress;
	std::wstring header( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	str.str( std::wstring() );
	str.clear();
	str << std::dec << blockInfo.BlockSize;
	header = std::wstring( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	str.str( std::wstring() );
	str.clear();
	str << std::hex << protectionConverter( blockInfo.BlockProtection );
	header = std::wstring( str.str() );
	drawNextFieldInRow( hdc, header.data() );

	currentY += fieldHeight;
}

std::wstring CDrawer::protectionConverter( DWORD protection )
{
	switch( protection & ~( PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE ) ) {
		case PAGE_READONLY:          
			return L"- R - -";
		case PAGE_READWRITE:
			return L"- R W -";
		case PAGE_WRITECOPY:         
			return L"- R W C";
		case PAGE_EXECUTE:           
			return L"E - - -";
		case PAGE_EXECUTE_READ:      
			return L"E R - -";
		case PAGE_EXECUTE_READWRITE: 
			return L"E R W -";
		case PAGE_EXECUTE_WRITECOPY: 
			return L"E R W C";
		case PAGE_NOACCESS:          
		default:
			return L"- - - -";
	}
}