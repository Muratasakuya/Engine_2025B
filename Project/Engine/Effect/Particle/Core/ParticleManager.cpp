#include "ParticleManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Utility/GameTimer.h>

//============================================================================
//	ParticleManager classMethods
//============================================================================

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ParticleManager::Init(Asset* asset, ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	// 各メインモジュール初期化
	gpuUpdater_ = std::make_unique<GPUParticleUpdater>();
	gpuUpdater_->Init(device, srvDescriptor, shaderCompiler);

	renderer_ = std::make_unique<ParticleRenderer>();
	renderer_->Init(device, asset, srvDescriptor, shaderCompiler);
}

void ParticleManager::Update(DxCommand* dxCommand) {

	if (systems_.empty()) {
		return;
	}

	// すべてのシステムを更新
	for (auto& system : systems_) {
		for (auto& group : system->GetGPUGroup()) {

			// グループ更新
			group.group.Update();
			// GPU更新
			gpuUpdater_->Update(group.group, dxCommand);
		}
	}
}

void ParticleManager::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	if (systems_.empty()) {
		return;
	}

	// すべてのシステムを更新
	for (const auto& system : systems_) {
		for (const auto& group : system->GetGPUGroup()) {

			// 描画処理
			renderer_->Rendering(debugEnable, group.group, sceneBuffer, dxCommand);
		}
	}
}

void ParticleManager::AddSystem() {

	// 名前の設定
	const std::string name = "particleSystem" + std::to_string(++nextSystemId_);
	// 初期化して追加
	std::unique_ptr<ParticleSystem> system = std::make_unique<ParticleSystem>();
	system->Init(device_, asset_, name);
	systems_.emplace_back(std::move(system));
}

void ParticleManager::RemoveSystem() {

	// 選択中のシステムを削除
	if (0 <= selectedSystem_ && selectedSystem_ < static_cast<int>(systems_.size())) {

		systems_.erase(systems_.begin() + selectedSystem_);

		// 未選択状態にする
		selectedSystem_ = -1;
		renamingSystem_ = -1;
	}
}

void ParticleManager::ImGui() {

	ImGui::Columns(2, "##split", true);
	// 左側
	{
		// 追加・削除ボタン
		if (ImGui::Button("+##ParticleManager")) {

			AddSystem();
		}
		ImGui::SameLine();
		if (ImGui::Button("-##ParticleManager")) {

			RemoveSystem();
		}

		// ツリー表示
		for (int sytemI = 0; sytemI < systems_.size(); ++sytemI) {

			ParticleSystem& system = *systems_[sytemI];
			bool open = ImGui::TreeNodeEx((void*)(intptr_t)sytemI,
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_OpenOnDoubleClick,
				"%s", system.GetName().c_str());

			// 選択後、ダブルクリックで改名
			if (ImGui::IsItemClicked()) {

				selectedSystem_ = sytemI;
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {

				renamingSystem_ = sytemI;
				strncpy_s(renameBuffer_, sizeof(renameBuffer_), system.GetName().c_str(), _TRUNCATE);
			}

			// 改名中ならInputTextを重ね描画
			if (renamingSystem_ == sytemI) {

				ImGui::SameLine();
				if (ImGui::InputText("##renameSystem", renameBuffer_, sizeof(renameBuffer_),
					ImGuiInputTextFlags_EnterReturnsTrue)) {

					system.SetName(renameBuffer_);
					renamingSystem_ = -1;
				}
			}

			// Group表示
			if (open) {
				for (int groupI = 0; groupI < system.GetGPUGroup().size(); ++groupI) {

					auto nodeId = reinterpret_cast<void*>(static_cast<intptr_t>((sytemI << 16) | groupI));
					const std::string& gname = system.GetGroupName(groupI);
					ImGui::TreeNodeEx(nodeId,
						ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
						"%s", gname.c_str());

					// Group選択、改名はSystem側で処理
					if (ImGui::IsItemClicked()) {

						selectedSystem_ = sytemI;
						system.SelectGroup(groupI);
					}
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {

						system.BeginRenameGroup(groupI, gname.c_str());
					}
				}
				ImGui::TreePop();
			}
		}
	}

	// 右側
	ImGui::NextColumn();

	if (0 <= selectedSystem_ && selectedSystem_ < static_cast<int>(systems_.size())) {

		systems_[selectedSystem_]->ImGui();
	}
}