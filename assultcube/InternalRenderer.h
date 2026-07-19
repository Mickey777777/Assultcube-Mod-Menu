#pragma once
#include <windows.h>

bool InternalRenderer_Init(HWND hwnd);
void InternalRenderer_BeginFrame();
void InternalRenderer_EndFrame();