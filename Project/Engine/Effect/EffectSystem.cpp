#include "EffectSystem.h"

//============================================================================
//	EffectSystem classMethods
//============================================================================

EffectSystem* EffectSystem::instance_ = nullptr;

EffectSystem* EffectSystem::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new EffectSystem();
	}
	return instance_;
}

void EffectSystem::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void EffectSystem::ImGui() {

	// particleとtrailの両方の値操作を行えるようにする
	// particleとtrailの両方をここで追加、削除を行えて、
	// 保存も行えるようにする
}