#pragma once

//============================================================================
//	include
//============================================================================

// windows
#include <Windows.h>
// c++
#include <cstdint>

//============================================================================
//	WinApp class
//============================================================================
class WinApp {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	WinApp() = default;
	~WinApp() = default;

	void Create();

	bool ProcessMessage();

	//--------- accessor -----------------------------------------------------

	void SetFullscreen(bool fullscreen);

	HWND GetHwnd() const { return hwnd_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	HWND hwnd_;

	UINT windowStyle_;
	RECT windowRect_;

	//--------- functions ----------------------------------------------------

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void RegisterWindowClass();
};