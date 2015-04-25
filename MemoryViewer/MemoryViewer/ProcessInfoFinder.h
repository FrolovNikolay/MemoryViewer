// Автор: Николай Фролов.

// Описание: класс-мехазнизм для получения информации об устройстве виртуального адресного пространства процесса.

#pragma once

#include <Windows.h>
#include <vector>

// Информация об отдельном блоке.
struct CBlockInfo {
public:
	// Адрес начала блока.
	PVOID BlockBaseAddress;
	// Размер блока.
	SIZE_T BlockSize;
	// Тип защиты блока. PAGE_*
	DWORD BlockProtection;
	// MEM_* Free, Image, Mapped, Private.
	DWORD BlockTypeOfUsing;
};

// Информация об отдельном регионе.
struct CRegionInfo {
public:
	// Адрес начала региона.
	PVOID RegionBaseAddress;
	// Размер региона.
	SIZE_T RegionSize;
	// Тип защиты региона. PAGE_*
	DWORD RegionProtection;
	// MEM_ Free,Image,Mapped.
	DWORD RegionTypeOfUsing;
	// Число блоков с GUARD защитой - позволяет сделать предположение о стеке.
	DWORD NumberOfGuardedBlocks;
	// Список с информацией о блоках внутри региона.
	std::vector<CBlockInfo> Blocks;
};

typedef std::vector<CRegionInfo> TProcessInfo;

class CProcessInfoFinder {
public:
	// Получить информацию о регионах и блоках виртуального пространства процесса hProcess.
	static TProcessInfo FindProcessInfo( int processId );
private:
	
	// Размер грануляции.
	static DWORD granulationSize;
	// Проведена ли инициализация класса.
	static bool isInitiated;

	static bool initiate();

	static bool findRegionInfo( HANDLE processHandle, PVOID address, CRegionInfo& regionInfo );
};