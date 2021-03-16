#include <stdafx.h>
#include <string>
#include <mutex>
unsigned char ManagedWrapper::peekbyte(long long addr)
{
	uint8_t val = 0;
	
	g_MMU->LB_PAddr(static_cast<uint32_t>(addr), val);
	return val;
}

void ManagedWrapper::pokebyte(long long addr, unsigned char val)
{
	g_MMU->SB_PAddr(static_cast<uint8_t>(addr), val);
}

void ManagedWrapper::savesavestate()
{
	g_System->SaveState();
	
}
int ManagedWrapper::GetMemSize()
{
	return 0x007FFFFF;
}
int ManagedWrapper::GetVRAMSize()
{
	return /*VRAM_SIZE*/0x01000000;
}
void ManagedWrapper::loadsavestate()
{
	g_System->LoadState();
}
void ManagedWrapper::RelayToFlycastLog(std::string string)
{
	//ERROR_LOG(COMMON, string.c_str());
}
std::string ManagedWrapper::getstatepath()
{
	return g_Settings->LoadStringVal(GameRunning_InstantSaveFile);
}
