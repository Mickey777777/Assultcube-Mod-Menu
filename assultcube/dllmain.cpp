#include <windows.h>
#include "MinHook.h"
#include "InternalRenderer.h"
#include "Memory.h"
#include "imgui/imgui.h"

bool godMode = false;
bool infAmmo = false;
bool menuOpen = false;
bool prevMenuOpen = false;
uintptr_t baseAddr = 0;

typedef BOOL(WINAPI* tWglSwapBuffers)(HDC);
tWglSwapBuffers oWglSwapBuffers = nullptr;

typedef BOOL(WINAPI* tSetCursorPos)(int, int);
tSetCursorPos oSetCursorPos = nullptr;

BOOL WINAPI hkSetCursorPos(int x, int y) {
    if (menuOpen) return TRUE;
    return oSetCursorPos(x, y);
}

typedef BOOL(WINAPI* tClipCursor)(const RECT*);
tClipCursor oClipCursor = nullptr;

BOOL WINAPI hkClipCursor(const RECT* rc) {
    if (menuOpen) return oClipCursor(nullptr);
    return oClipCursor(rc);
}

typedef int(*tSDL_SetRelativeMouseMode)(int);
typedef int(*tSDL_WM_GrabInput)(int);
typedef int(*tSDL_ShowCursor)(int);
tSDL_SetRelativeMouseMode pSDL_SetRelativeMouseMode = nullptr;
tSDL_WM_GrabInput pSDL_WM_GrabInput = nullptr;
tSDL_ShowCursor pSDL_ShowCursor = nullptr;

void ResolveSDL() {
    if (HMODULE sdl2 = GetModuleHandleA("SDL2.dll")) {
        pSDL_SetRelativeMouseMode = (tSDL_SetRelativeMouseMode)GetProcAddress(sdl2, "SDL_SetRelativeMouseMode");
        pSDL_ShowCursor = (tSDL_ShowCursor)GetProcAddress(sdl2, "SDL_ShowCursor");
    }
    else if (HMODULE sdl1 = GetModuleHandleA("SDL.dll")) {
        pSDL_WM_GrabInput = (tSDL_WM_GrabInput)GetProcAddress(sdl1, "SDL_WM_GrabInput");
        pSDL_ShowCursor = (tSDL_ShowCursor)GetProcAddress(sdl1, "SDL_ShowCursor");
    }
}


void ApplyMouseState(bool open) {
    if (pSDL_SetRelativeMouseMode) pSDL_SetRelativeMouseMode(open ? 0 : 1);
    else if (pSDL_WM_GrabInput)    pSDL_WM_GrabInput(open ? 0 : 1);
    if (pSDL_ShowCursor)           pSDL_ShowCursor(0);
}

BOOL WINAPI hkWglSwapBuffers(HDC hdc) {
    static bool init = false;
    if (!init) {
        HWND hwnd = WindowFromDC(hdc);
        InternalRenderer_Init(hwnd);
        ResolveSDL();
        init = true;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        menuOpen = !menuOpen;
    }

    if (menuOpen) {
        ApplyMouseState(true);
    }
    else if (prevMenuOpen) {
        ApplyMouseState(false);
    }
    prevMenuOpen = menuOpen;

    InternalRenderer_BeginFrame();

    ImGui::GetIO().MouseDrawCursor = menuOpen;

    if (menuOpen) {
        ImGui::Begin("Mod Menu");
        ImGui::Checkbox("God Mode", &godMode);
        ImGui::Checkbox("Infinite Ammo", &infAmmo);
        ImGui::End();
    }
    InternalRenderer_EndFrame();

    if (godMode) setHealth(baseAddr, 99999999);
    if (infAmmo) setAmmo(baseAddr, 99999999);

    return oWglSwapBuffers(hdc);
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    baseAddr = (uintptr_t)GetModuleHandleA("ac_client.exe");

    MH_Initialize();

    void* target = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
    MH_CreateHook(target, &hkWglSwapBuffers, (void**)&oWglSwapBuffers);

    HMODULE user32 = GetModuleHandleA("user32.dll");
    void* pSetCursorPos = GetProcAddress(user32, "SetCursorPos");
    void* pClipCursor = GetProcAddress(user32, "ClipCursor");
    MH_CreateHook(pSetCursorPos, &hkSetCursorPos, (void**)&oSetCursorPos);
    MH_CreateHook(pClipCursor, &hkClipCursor, (void**)&oClipCursor);

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }
    return TRUE;
}