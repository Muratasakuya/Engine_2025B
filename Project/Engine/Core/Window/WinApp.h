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

	void Create(uint32_t width, uint32_t height, const wchar_t* title);

	bool ProcessMessage();

	//--------- accessor -----------------------------------------------------

	void SetFullscreen(bool fullscreen);

	HWND GetHwnd() const { return hwnd_; }

	static uint32_t GetWindowWidth() { return windowWidth_; }
	static uint32_t GetWindowHeight() { return windowHeight_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static uint32_t windowWidth_;
	static uint32_t windowHeight_;

	HWND hwnd_;

	UINT windowStyle_;
	RECT windowRect_;

	bool isFullscreen_;

	//--------- functions ----------------------------------------------------

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void RegisterWindowClass();

	void CheckAspectRatio(uint32_t width, uint32_t height);
};