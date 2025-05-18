#include "WinApp.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Config.h>

// imgui
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma comment(lib,"winmm.lib")

//============================================================================
//	WinApp classMethods
//============================================================================

void WinApp::Create() {

	RegisterWindowClass();

	RECT wrc = {};
	wrc.right = Config::kWindowWidth;
	wrc.bottom = Config::kWindowHeight;

	windowStyle_ = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	AdjustWindowRect(&wrc, windowStyle_, false);

	hwnd_ = CreateWindow(
		L"WindowClass",
		Config::kWindowTitle,
		windowStyle_,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		nullptr);

	ShowWindow(hwnd_, SW_SHOW);

	// 1920*1080にした時のウィンドウの座標
	// ずれないように補正
	if (wrc.right == 1920 && wrc.bottom == 1080) {

		SetWindowPos(hwnd_, nullptr, -8, -2, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

bool WinApp::ProcessMessage() {

	MSG msg{};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {

		return true;
	}

	return false;
}

void WinApp::SetFullscreen(bool fullscreen) {

	if (isFullscreen_ != fullscreen) {
		if (fullscreen) {

			// 元の状態を覚えておく
			GetWindowRect(hwnd_, &windowRect_);

			// 仮想フルスクリーン化
			SetWindowLong(
				hwnd_, GWL_STYLE,
				windowStyle_ &
				~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenRect{ 0 };
			HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info;
			info.cbSize = sizeof(info);
			GetMonitorInfo(monitor, &info);
			fullscreenRect.right = info.rcMonitor.right - info.rcMonitor.left;
			fullscreenRect.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

			SetWindowPos(
				hwnd_, HWND_TOPMOST, fullscreenRect.left, fullscreenRect.top, fullscreenRect.right,
				fullscreenRect.bottom, SWP_FRAMECHANGED | SWP_NOACTIVATE);
			ShowWindow(hwnd_, SW_MAXIMIZE);

		} else {
			// 通常ウィンドウに戻す
			SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);

			SetWindowPos(
				hwnd_, HWND_NOTOPMOST, windowRect_.left, windowRect_.top,
				windowRect_.right - windowRect_.left, windowRect_.bottom - windowRect_.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(hwnd_, SW_NORMAL);
		}
	}

	isFullscreen_ = fullscreen;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	// ImGuiマウス有効
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::RegisterWindowClass() {

	// Window Procedure
	WNDCLASS wc{};
	// ウィンドウプロシージャ(Window Procedure)
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc);
}