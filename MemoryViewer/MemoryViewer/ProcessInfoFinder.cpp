// Автор: Николай Фролов.

#include <ProcessInfoFinder.h>

bool CProcessInfoFinder::isInitiated = false;

TProcessMemoryInfo CProcessInfoFinder::FindProcessInfo( int processId )
{
	if( !isInitiated && !initiate() ) {
		throw std::runtime_error( "Access denied." );
	}

	HANDLE processHandle = ::OpenProcess( PROCESS_QUERY_INFORMATION, false, processId );
	if( processHandle == 0 ) {
		// Процесс уже не работает, возвращаем пустой результат.
		return TProcessMemoryInfo();
	}

	PVOID currentAddress = 0;
	bool isNotFinished = true;
	TProcessMemoryInfo result;
	while( isNotFinished ) {
		CRegionInfo tmpRegionInfo;
		isNotFinished = findRegionInfo( processHandle, currentAddress, tmpRegionInfo );
		if( isNotFinished ) {
			result.push_back( tmpRegionInfo );
			currentAddress = static_cast<PBYTE>( currentAddress ) + tmpRegionInfo.RegionSize;
		}
	}
	return result;
}

// Инициация грануляции и получения необходимых для работы прав.
bool CProcessInfoFinder::initiate()
{
	bool fuctionSucceed = true;

	// Выставляем права.
	HANDLE accessToken;
	if( ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &accessToken ) ) {
		TOKEN_PRIVILEGES privileges;
		privileges.PrivilegeCount = 1;
		::LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &privileges.Privileges[0].Luid ); 
		privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		::AdjustTokenPrivileges( accessToken, false, &privileges, sizeof( privileges ), 0, 0 );
		fuctionSucceed = GetLastError() == ERROR_SUCCESS;
	}
	::CloseHandle( accessToken );

	return fuctionSucceed;
}


bool CProcessInfoFinder::findRegionInfo( HANDLE processHandle, PVOID address, CRegionInfo& regionInfo )
{
	MEMORY_BASIC_INFORMATION mbi;
	if( ::VirtualQueryEx( processHandle, address, &mbi, sizeof( mbi ) ) != sizeof( mbi ) ) {
		return false;
	}
	regionInfo.RegionBaseAddress = mbi.BaseAddress;
	regionInfo.RegionSize = 0;
	regionInfo.RegionProtection = mbi.AllocationProtect;
	regionInfo.RegionTypeOfUsing = mbi.State;
	if( mbi.State == MEM_FREE ) {
		regionInfo.NumberOfGuardedBlocks = 0;
		regionInfo.RegionSize = mbi.RegionSize;
	} else {
		PVOID blockAddress = mbi.AllocationBase;
		while( true ) {
			if( ::VirtualQueryEx( processHandle, blockAddress, &mbi, sizeof( mbi ) ) != sizeof( mbi ) ) {
				// Не можем получить информацию о блоке.
				break;
			}
			if( mbi.AllocationBase != regionInfo.RegionBaseAddress ) {
				// Вышли за предел региона.
				break;
			}

			CBlockInfo tmpBlockInfo;
			tmpBlockInfo.BlockBaseAddress = mbi.BaseAddress;
			tmpBlockInfo.BlockSize = mbi.RegionSize;
			tmpBlockInfo.BlockTypeOfUsing = mbi.State;
			if( mbi.State != MEM_FREE ) {
				tmpBlockInfo.BlockProtection = mbi.Protect;
			}
			if( ( mbi.Protect & PAGE_GUARD ) == PAGE_GUARD ) {
				++regionInfo.NumberOfGuardedBlocks;;
			}
			regionInfo.Blocks.push_back( tmpBlockInfo );
			regionInfo.RegionSize += mbi.RegionSize;
			blockAddress = reinterpret_cast<PVOID>( reinterpret_cast<PBYTE>( blockAddress ) + mbi.RegionSize );
		}
		// Итерация по блокам.
		// ..
	}
	return true;
}