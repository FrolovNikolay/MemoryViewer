// Автор: Николай Фролов.

// Описание: класс-мехазнизм для получения информации об устройстве виртуального адресного пространства процесса.

#pragma once

#include <Windows.h>


struct CProccessInfo {
};

class CProccessInfoFinder {
public:
	static CProccessInfo FindProccessInfo( int pId );
private:

};