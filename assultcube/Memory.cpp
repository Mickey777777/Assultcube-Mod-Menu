#include "Memory.h"

bool setHealth(uintptr_t baseAddr, int health) {
	uintptr_t healthBase = baseAddr + 0x0017E254;
	uintptr_t nextAddr = *(uintptr_t*)healthBase;

	uintptr_t healthAddr = nextAddr + 0xEC;
	*(int*)healthAddr = health;

	return true;
}

bool setAmmo(uintptr_t baseAddr, int ammo) {
	uintptr_t ammoBase = baseAddr + 0x0018AC00;
	uintptr_t nextAddr = *(uintptr_t*)ammoBase;

	uintptr_t ammoAddr = nextAddr + 0x140;
	*(int*)ammoAddr = ammo;

	return true;
}
