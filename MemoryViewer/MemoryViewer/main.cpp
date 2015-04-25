// Автор: Николай Фролов.

#include <ProcessInfoFinder.h>
#include <fstream>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	int pId = 100;
	TProcessInfo info = CProcessInfoFinder::FindProcessInfo( ::GetCurrentProcessId() );
	std::ofstream output( "output" );
	for( auto& piece : info ) {
		output << piece.RegionBaseAddress << " " << piece.RegionSize << std::endl;
	}
	return 0;
}