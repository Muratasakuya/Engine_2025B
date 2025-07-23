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

void ParticleSystem::ImGui() {

	ImGui::Columns(2, "##systemSplit", true);

	// タイプ選択
	EnumAdapter<ParticleType>::Combo("particleType", &particleType_);
	EnumAdapter<ParticlePrimitiveType>::Combo("primitiveType", &primitiveType_);

	// groupの追加、削除
	if (ImGui::Button("+##ParticleSystem")) {

		AddGroup();
	}
	ImGui::SameLine();
	if (ImGui::Button("-##ParticleSystem")) {

		RemoveGroup();
	}

	for (int groupI = 0; groupI < gpuGroups_.size(); ++groupI) {

		bool selected = (selectedGroup_ == groupI);
		if (ImGui::Selectable(gpuGroups_[groupI].name.c_str(), selected,
			ImGuiSelectableFlags_AllowDoubleClick)) {

			selectedGroup_ = groupI;
		}

		// ダブルクリックで改名
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {

			renamingGroup_ = groupI;
			strncpy_s(renameBuffer_, sizeof(renameBuffer_), gpuGroups_[groupI].name.c_str(), _TRUNCATE);
		}

		// 改名中
		if (renamingGroup_ == groupI) {
			ImGui::SameLine();
			if (ImGui::InputText("##renameGrp", renameBuffer_, sizeof(renameBuffer_),
				ImGuiInputTextFlags_EnterReturnsTrue)) {

				gpuGroups_[groupI].name = renameBuffer_;
				renamingGroup_ = -1;
			}
		}
	}

	// group内処理
	ImGui::NextColumn();
	if (0 <= selectedGroup_ && selectedGroup_ < static_cast<int>(gpuGroups_.size())) {

		gpuGroups_[selectedGroup_].group.ImGui(device_);
	}
	ImGui::Columns(1);
}

void ParticleSystem::BeginRenameGroup(int index, const char* name) {

	renamingGroup_ = index;
	strncpy_s(renameBuffer_, sizeof(renameBuffer_), name, _TRUNCATE);
}