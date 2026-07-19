#include <windows.h>
#include <tlhelp32.h>
#include <memoryapi.h>
#include <iostream>
#include "Renderer.h"
#include "imgui.h"

DWORD FindPidByName(const wchar_t* name) {
	DWORD pid = 0xFFFFFFFF;
	PROCESSENTRY32 pe;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	pe.dwSize = sizeof(PROCESSENTRY32);

	Process32First(hSnapShot, &pe);
	do {
		if (!_wcsicmp(pe.szExeFile, name)) {
			pid = pe.th32ProcessID;
			break;
		}
	}while(Process32Next(hSnapShot, &pe));

	CloseHandle(hSnapShot);
	return pid;
}

uintptr_t GetModuleBase(DWORD pid, const wchar_t* name) {
	uintptr_t baseAddr = 0x0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);

	if (hSnap == INVALID_HANDLE_VALUE) return 0;

	Module32First(hSnap, &me);
	do {
		if (!_wcsicmp(me.szModule, name)) {
			baseAddr = (uintptr_t)me.modBaseAddr;
			break;
		}
	} while (Module32Next(hSnap, &me));

	CloseHandle(hSnap);
	return baseAddr;
}

bool setHealth(HANDLE hProcess, uintptr_t baseAddr, int health) {
	uintptr_t healthBase = baseAddr + 0x0017E254;
	uintptr_t nextAddr = 0;
	ReadProcessMemory(hProcess, (LPCVOID)healthBase, &nextAddr, sizeof(nextAddr), nullptr);

	uintptr_t healthAddr = nextAddr + 0xEC;
	WriteProcessMemory(hProcess, (LPVOID)healthAddr, &health, sizeof(health), nullptr);

	return true;
}

bool setAmmo(HANDLE hProcess, uintptr_t baseAddr, int ammo) {
	uintptr_t ammoBase = baseAddr + 0x0018AC00;
	uintptr_t nextAddr = 0;
	ReadProcessMemory(hProcess, (LPCVOID)ammoBase, &nextAddr, sizeof(nextAddr), nullptr);

	uintptr_t ammoAddr = nextAddr + 0x140;
	WriteProcessMemory(hProcess, (LPVOID)ammoAddr, &ammo, sizeof(ammo), nullptr);

	return true;
}

int main() {
	DWORD pid = 0xFFFFFFFF;
	const wchar_t* processName = L"ac_client.exe";
	pid = FindPidByName(processName);
	uintptr_t baseAddr = GetModuleBase(pid, processName);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	bool godMode = false;
	bool infAmmo = false;

	if (!Renderer_Init()) {
		return 1;
	}

	while (Renderer_BeginFrame()) {
		ImGui::Begin("AssaultCube Mod Menu");
		ImGui::Checkbox("God Mode", &godMode);
		ImGui::Checkbox("Infinite Ammo", &infAmmo);
		ImGui::End();

		if (godMode) setHealth(hProcess, baseAddr, 99999999);
		if (infAmmo) setAmmo(hProcess, baseAddr, 99999999);

		Renderer_EndFrame();
	}
	
	Renderer_Shutdown();
	return 0;
}