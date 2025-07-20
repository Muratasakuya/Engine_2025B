#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// 注意
// EffectSystemに処理が固まりすぎて責務が重くならないようにすること

//============================================================================
//	EffectSystem class
//============================================================================
class EffectSystem :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void ImGui() override;

	void Rendering() {};

	//--------- accessor -----------------------------------------------------

	// singleton
	static EffectSystem* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static EffectSystem* instance_;

	//--------- functions ----------------------------------------------------

	EffectSystem() : IGameEditor("EffectSystem") {}
	~EffectSystem() = default;
	EffectSystem(const EffectSystem&) = delete;
	EffectSystem& operator=(const EffectSystem&) = delete;
};