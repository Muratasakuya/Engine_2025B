#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Scene/Light/PunctualLight.h>

//============================================================================
//	LightManager class
//============================================================================
class LightManager :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LightManager() : IGameEditor("lightManager") {};
	~LightManager() = default;

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetLight(PunctualLight* gameLight);

	PunctualLight* GetLight() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::optional<PunctualLight*> gameLight_;
};