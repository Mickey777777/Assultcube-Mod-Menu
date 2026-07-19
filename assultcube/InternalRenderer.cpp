#include "InternalRenderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

void InternalRenderer_BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void InternalRenderer_EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static WNDPROC oWndProc = nullptr;
extern bool menuOpen;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK hkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (menuOpen) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
        if (ImGui::GetIO().WantCaptureMouse && msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
            return true;
        }
    }

    return CallWindowProc(oWndProc, hwnd, msg, wParam, lParam);
}

bool InternalRenderer_Init(HWND hwnd) {
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
    return true;
}
