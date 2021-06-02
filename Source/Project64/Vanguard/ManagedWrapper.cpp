#include <stdafx.h>
#include <string>
#include <mutex>
extern "C" __declspec(dllexport)
unsigned char ManagedWrapper_peekbyte(long long addr)
{
	uint8_t val = 0;
	g_MMU->LB_PAddr(static_cast<uint32_t>(addr), val);
	printf("Value at 0x%X is %X", static_cast<uint32_t>(addr), val);
	return val;
}
extern "C" __declspec(dllexport)
void ManagedWrapper_pokebyte(long long addr, unsigned char val)
{
	g_MMU->SB_PAddr(static_cast<uint32_t>(addr), val);
}
extern "C" __declspec(dllexport)
void ManagedWrapper_savesavestate(const char* SaveFile)
{
	g_Settings->SaveString(GameRunning_InstantSaveFile, std::string(SaveFile));
	g_BaseSystem->ExternalEvent(SysEvent_PauseCPU_SaveGame);
	g_BaseSystem->ExternalEvent(SysEvent_SaveMachineState);
	g_BaseSystem->ExternalEvent(SysEvent_ResumeCPU_SaveGame);
	
}
extern "C" __declspec(dllexport)
int ManagedWrapper_GetMemSize()
{
	return 0x007FFFFF;
}
extern "C" __declspec(dllexport)
int ManagedWrapper_GetVRAMSize()
{
	return /*VRAM_SIZE*/0x01000000;
}
extern "C" __declspec(dllexport)
void ManagedWrapper_loadsavestate(const char* SaveFile)
{
	g_System->LoadState(SaveFile);
}
extern "C" __declspec(dllexport)
void ManagedWrapper_RelayToFlycastLog(std::string string)
{
	//ERROR_LOG(COMMON, string.c_str());
}
extern "C" __declspec(dllexport)
const char* ManagedWrapper_getstatepath()
{
	return "";
}
//also from MainMenu.cpp
stdstr ManagedWrapperHelper::GetFileLastMod(const CPath& FileName)
{
	HANDLE hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return "";
	}
	FILETIME CreationTime, LastAccessTime, LastWriteTime;
	stdstr LastMod;
	if (GetFileTime(hFile, &CreationTime, &LastAccessTime, &LastWriteTime))
	{
		SYSTEMTIME stUTC, stLocal;

		// Convert the last-write time to local time.
		FileTimeToSystemTime(&LastWriteTime, &stUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

		LastMod.Format(" [%d/%02d/%02d %02d:%02d]", stLocal.wYear, stLocal.wMonth, stLocal.wDay, stLocal.wHour, stLocal.wMinute);
	}
	CloseHandle(hFile);

	return LastMod;
}