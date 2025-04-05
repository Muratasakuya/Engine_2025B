#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Window/WinApp.h>
#include <Engine/Core/Graphics/GraphicsCore.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Editor/ImGuiEditor.h>
#include <Game/Scene/Manager/SceneManager.h>
#include <Game/Camera/Manager/CameraManager.h>

// component
#include <Engine/Component/Manager/TransformManager.h>
#include <Engine/Component/Manager/MaterialManager.h>
#include <Engine/Component/Manager/SpriteComponentManager.h>

// directX
#include <dxgidebug.h>
#include <dxgi1_6.h>
// c++
#include <memory>

//============================================================================
//	Framework class
//============================================================================
class Framework {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Framework(uint32_t width, uint32_t height, const wchar_t* title);
	~Framework() = default;

	void Run();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<WinApp> winApp_;

	std::unique_ptr<GraphicsCore> graphicsCore_;

	std::unique_ptr<Asset> asset_;

	std::unique_ptr<ImGuiEditor> imguiEditor_;

	std::unique_ptr<SceneManager> sceneManager_;

	std::unique_ptr<CameraManager> cameraManager_;

	// componentManagers

	// 3D
	std::unique_ptr<Transform3DManager> transform3DComponentManager_;
	std::unique_ptr<MaterialManager> materialManager_;

	// 2D
	std::unique_ptr<Transform2DManager> transform2DComponentManager_;
	std::unique_ptr<SpriteMaterialManager> spriteMaterialManager_;
	std::unique_ptr<SpriteComponentManager> spriteComponentManager_;

	//--------- functions ----------------------------------------------------

	void InitDirectX(uint32_t width, uint32_t height);
	void InitComponent();

	void Update();
	void UpdateScene();

	void Draw();

	void Finalize();

	//--------- LeakChecker ----------------------------------------------------

	struct LeakChecker {

		~LeakChecker() {

			ComPtr<IDXGIDebug1> debug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {

				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
	};
	LeakChecker leakChecker_;
};