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
	// GPU、発生処理しか行わない
	for (auto& group : gpuGroups_) {

		group.group.Update();
	}
	// CPU
	for (auto& group : cpuGroups_) {

		group.group.Update();
	}
}

void ParticleSystem::AddGroup() {

	// タイプに応じて作成
	if (particleType_ == ParticleType::CPU) {

		// 追加
		auto& group = cpuGroups_.emplace_back();
		// 名前の設定
		group.name = "particle" + std::to_string(++nextGroupId_);
		// 作成
		group.group.Create(device_, primitiveType_);
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
	if (selected_.index < 0) {
		return;
	}

	if (selected_.type == ParticleType::GPU) {

		gpuGroups_.erase(gpuGroups_.begin() + selected_.index);
	}
	else if(selected_.type == ParticleType::CPU) {

		cpuGroups_.erase(cpuGroups_.begin() + selected_.index);
	}
	// 未選択状態にする
	selected_.index = -1;
	renaming_.index = -1;
}

void ParticleSystem::ImGuiGroupAdd() {

	EditLayout();

	ImGui::PushItemWidth(comboWidth_);

	EnumAdapter<ParticleType>::Combo("Type", &particleType_);
	EnumAdapter<ParticlePrimitiveType>::Combo("Primitive", &primitiveType_);

	ImGui::PopItemWidth();

	// 追加と削除
	if (ImGui::Button("+Group", buttonSize_)) {

		AddGroup();
	}
	ImGui::SameLine();
	if (ImGui::Button("-Group", buttonSize_)) {

		RemoveGroup();
	}
}

void ParticleSystem::ImGuiGroupSelect() {

	int id = 0;
	auto drawItem = [&](auto& vec, ParticleType type) {
		for (int i = 0; i < static_cast<int>(vec.size()); ++i, ++id) {

			const bool isSelected = (selected_.type == type && selected_.index == i);

			// 表示名
			std::string label = std::format("[{}] {}", EnumAdapter<ParticleType>::ToString(type), vec[i].name);
			ImGui::PushID(id);
			if (ImGui::Selectable(label.c_str(), isSelected,
				ImGuiSelectableFlags_AllowDoubleClick)) {

				selected_ = { type, i };
			}

			// ダブルクリックで改名開始
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {

				renaming_ = { type, i };
				strcpy_s(renameBuffer_, vec[i].name.c_str());
			}

			// 改名中
			if (renaming_.type == type && renaming_.index == i) {

				ImGui::SameLine();
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::InputText("##rename", renameBuffer_, sizeof(renameBuffer_),
					ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) ||
					(!ImGui::IsItemActive() && ImGui::IsItemDeactivated())) {

					vec[i].name = renameBuffer_;
					renaming_ = { ParticleType::GPU, -1 };
				}
			}
			ImGui::PopID();
		}
		};

	drawItem(gpuGroups_, ParticleType::GPU);
	drawItem(cpuGroups_, ParticleType::CPU);
}

void ParticleSystem::ImGuiSystemParameter() {

	ImGui::TextDisabled("...");
}

void ParticleSystem::ImGuiSelectedGroupEditor() {

	ImGui::PushItemWidth(itemWidth_);

	if (0 <= selected_.index) {
		if (selected_.type == ParticleType::GPU) {

			gpuGroups_[selected_.index].group.ImGui(device_);
		} else if (selected_.type == ParticleType::CPU) {

			cpuGroups_[selected_.index].group.ImGui();
		}
	}

	ImGui::PopItemWidth();
}

void ParticleSystem::EditLayout() {

	ImGui::Begin("ParticleSystem");

	ImGui::DragFloat("comboWidth_", &comboWidth_, 0.1f);
	ImGui::DragFloat("itemWidth_", &itemWidth_, 0.1f);
	ImGui::DragFloat2("buttonSize_", &buttonSize_.x, 0.1f);

	ImGui::End();
}