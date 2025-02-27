
//============================================================================
//	include
//============================================================================
#include <Engine/Core/Framework.h>

//============================================================================
//	windowConfig
//============================================================================

// windowSize
const constexpr uint32_t kWindowWidth = 1280;
const constexpr uint32_t kWindowHeight = 720;

// windowTitle
const constexpr wchar_t* kWindowTitle = L"Engine";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<Framework> game = std::make_unique<Framework>(kWindowWidth, kWindowHeight, kWindowTitle);
	game->Run();

	return 0;
}