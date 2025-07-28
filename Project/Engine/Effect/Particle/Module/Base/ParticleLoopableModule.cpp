#include "ParticleLoopableModule.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ParticleLoopableModule classMethods
//============================================================================

void ParticleLoopableModule::ImGuiLoopParam() {

	ImGui::SeparatorText("Loop");

	ImGui::DragInt("loopCount", &loopCount_, 1, 1, 64);
	EnumAdapter<ParticleLoop::Type>::Combo("loopType", &loopType_);
}