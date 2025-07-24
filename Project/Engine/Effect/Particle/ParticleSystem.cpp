#include "ParticleSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Lib/Adapter/EnumAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	ParticleSystem classMethods
//============================================================================

void ParticleSystem::Init(ID3D12Device* device,
	Asset* asset, const std::string& name) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	name_ = name;
}

void ParticleSystem::Update() {

	if (gpuGroups_.empty()) {
		return;
	}

	// 所持しているパーティクルの更新
	for (auto& group : gpuGroups_) {

		group.group.Update();
	}
}

void ParticleSystem::AddGroup() {

	// タイプに応じて作成
	if (particleType_ == ParticleType::CPU) {

		// 今は何もないよ～ん
	} else if (particleType_ == ParticleType::GPU) {

		// 追加
		auto& group = gpuGroups_.emplace_back();
		// 名前の設定
		group.name = "particle" + std::to_string(++nextGroupId_);
		// 作成
		group.group.Create(device_, primitiveType_);
	}
}

void ParticleSystem::RemoveGroup() {

	// 選択中のグループを削除
	if (0 <= selectedGroup_ && selectedGroup_ < static_cast<int>(gpuGroups_.size())) {

		gpuGroups_.erase(gpuGroups_.begin() + selectedGroup_);

		// 未選択状態にする
		selectedGroup_ = -1;
		renamingGroup_ = -1;
	}
}

void ParticleSystem::ImGuiGroupAdd() {

	EnumAdapter<ParticleType>::Combo("Type", &particleType_);
	EnumAdapter<ParticlePrimitiveType>::Combo("Primitive", &primitiveType_);

	// 追加と削除
	if (ImGui::Button("+Group")) {

		AddGroup();
	}
	ImGui::SameLine();
	if (ImGui::Button("-Group")) {

		RemoveGroup();
	}
}

void ParticleSystem::ImGuiGroupSelect() {

	for (int i = 0; i < gpuGroups_.size(); ++i) {

		bool selected = (selectedGroup_ == i);
		if (ImGui::Selectable(gpuGroups_[i].name.c_str(), selected,
			ImGuiSelectableFlags_AllowDoubleClick)) {

			selectedGroup_ = i;
		}

		// ダブルクリックで改名開始
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {

			BeginRenameGroup(i, gpuGroups_[i].name.c_str());
		}

		// 改名入力
		if (renamingGroup_ == i) {

			ImGui::SameLine();

			ImGui::PushID(i);
			ImGui::SetNextItemWidth(-FLT_MIN);

			ImGuiInputTextFlags flags =
				ImGuiInputTextFlags_AutoSelectAll |
				ImGuiInputTextFlags_EnterReturnsTrue;
			bool done = ImGui::InputText("##renameGrp", renameBuffer_,
				sizeof(renameBuffer_), flags);
			if (ImGui::IsItemActivated()) {

				ImGui::SetKeyboardFocusHere(-1);
			}

			if (done || (!ImGui::IsItemActive() && ImGui::IsItemDeactivated())) {

				gpuGroups_[i].name = renameBuffer_;
				renamingGroup_ = -1;
			}
			ImGui::PopID();
		}
	}
}

void ParticleSystem::ImGuiSystemParameter() {

	ImGui::TextDisabled("...");
}

void ParticleSystem::ImGuiSelectedGroupEditor() {

	if (0 <= selectedGroup_ && selectedGroup_ < static_cast<int>(gpuGroups_.size())) {

		gpuGroups_[selectedGroup_].group.ImGui(device_);
	}
}

void ParticleSystem::BeginRenameGroup(int index, const char* name) {

	renamingGroup_ = index;
	strncpy_s(renameBuffer_, sizeof(renameBuffer_), name, _TRUNCATE);
}