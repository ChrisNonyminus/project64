#include <stdafx.h>
#include <string>
#include <mutex>
extern "C" __declspec(dllexport)
unsigned char ManagedWrapper_peekbyte(long long addr)
{
	uint8_t val = 0;
	
	g_MMU->LB_VAddr(static_cast<uint32_t>(addr), val);
	return val;
}
extern "C" __declspec(dllexport)
void ManagedWrapper_pokebyte(long long addr, unsigned char val)
{
	g_MMU->SB_VAddr(static_cast<uint32_t>(addr), val);
	
}
extern "C" __declspec(dllexport)
void ManagedWrapper_savesavestate()
{
	g_BaseSystem->ExternalEvent(SysEvent_SaveMachineState);
	
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
void ManagedWrapper_loadsavestate()
{
	g_BaseSystem->ExternalEvent(SysEvent_LoadMachineState);
}
extern "C" __declspec(dllexport)
void ManagedWrapper_RelayToFlycastLog(std::string string)
{
	//ERROR_LOG(COMMON, string.c_str());
}
extern "C" __declspec(dllexport)
const char* ManagedWrapper_getstatepath()
{
	//code taken from MainMenu.cpp
	std::string SlotName = std::string(GS(MENU_SLOT_DEFAULT));
	if (!g_Settings->LoadBool(GameRunning_CPU_Running)) { return SlotName.c_str(); }
	std::string LastSaveTime;
	CPath FileName(g_Settings->LoadStringVal(Directory_InstantSave).c_str(), "");
	if (g_Settings->LoadBool(Setting_UniqueSaveDir))
	{
		FileName.AppendDirectory(g_Settings->LoadStringVal(Game_UniqueSaveDir).c_str());
	}
	FileName.NormalizePath(CPath(CPath::MODULE_DIRECTORY));
	FileName.SetNameExtension(stdstr_f("%s.pj", g_Settings->LoadStringVal(Rdb_GoodName).c_str()).c_str());
	if (g_Settings->LoadDword(Setting_AutoZipInstantSave))
	{
		CPath ZipFileName(FileName.GetDriveDirectory(), stdstr_f("%s.zip", FileName.GetNameExtension().c_str()).c_str());
		LastSaveTime = ManagedWrapperHelper::GetFileLastMod(ZipFileName);
	}
	if (LastSaveTime.empty())
	{
		LastSaveTime = ManagedWrapperHelper::GetFileLastMod(FileName);
		FileName.SetNameExtension(stdstr_f("%s.pj%d", g_Settings->LoadStringVal(Game_GameName).c_str(), 0).c_str());
		if (g_Settings->LoadBool(Setting_AutoZipInstantSave))
		{
			CPath ZipFileName(FileName.GetDriveDirectory(), stdstr_f("%s.zip", FileName.GetNameExtension().c_str()).c_str());
			LastSaveTime = ManagedWrapperHelper::GetFileLastMod(ZipFileName);
		}
		if (LastSaveTime.empty())
		{
			LastSaveTime = ManagedWrapperHelper::GetFileLastMod(FileName);
		}
	}
	SlotName += LastSaveTime;
	return SlotName.c_str();
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