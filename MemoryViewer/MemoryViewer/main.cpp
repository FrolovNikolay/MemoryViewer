// Автор: Николай Фролов.

#include <ProccessInfoFinder.h>

int main() {
	int pId = 100;
	CProccessInfo info = CProccessInfoFinder::FindProccessInfo( pId );
	return 0;
}