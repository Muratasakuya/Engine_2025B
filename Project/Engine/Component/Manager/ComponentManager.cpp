#include "ComponentManager.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	ComponentManager classMethods
//============================================================================

ComponentManager* ComponentManager::instance_ = nullptr;

ComponentManager* ComponentManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ComponentManager();
	}
	return instance_;
}

void ComponentManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ComponentManager::Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	srvManager_ = nullptr;
	srvManager_ = srvManager;

	entityManager_ = std::make_unique<EntityManager>();
	transform3DManager_ = std::make_unique<Transform3DManager>();
	materialManager_ = std::make_unique<MaterialManager>();
	modelComponentManager_ = std::make_unique<ModelComponentManager>();

	componentImGui_ = std::make_unique<ComponentImGui>();
	componentImGui_->Init(entityManager_.get(), transform3DManager_.get(),
		materialManager_.get(), modelComponentManager_.get());
}

void ComponentManager::Update() {

	// 各Componentの更新処理
	transform3DManager_->Update();
	materialManager_->Update();
}

Object3D ComponentManager::CreateObject3D(const std::string& modelName,
	const std::optional<std::string>& animationName, const std::string& objectName) {

	// EntityID発行
	EntityID id = entityManager_->CreateEntity(objectName);

	Transform3DComponent* transform = transform3DManager_->AddComponent(id, device_);
	std::vector<Material*> material = materialManager_->AddComponent(id, asset_->GetModelData(modelName).meshes.size(), device_);
	ModelComponent* model = modelComponentManager_->AddComponent(id,
		modelName, animationName, device_, asset_, srvManager_);
	std::function<void()>* imguiFunc = componentImGui_->AddComponent(id);

	object3Ds_[id] = {
		transform3DManager_->GetBuffer(id),
		materialManager_->GetBuffer(id),
		modelComponentManager_->GetComponent(id),
	};

	needsSorting_ = true;

	return { id,transform ,material,model,imguiFunc };
}

void ComponentManager::RemoveObject3D(EntityID id) {

	object3Ds_.erase(id);
	entityManager_->DestroyEntity(id);
	transform3DManager_->RemoveComponent(id);
	materialManager_->RemoveComponent(id);
	modelComponentManager_->RemoveComponent(id);
	componentImGui_->RemoveComponent(id);

	needsSorting_ = true;
}

void ComponentManager::InvalidateBlendModeCache() {

	needsSorting_ = true;
}

void ComponentManager::RebuildBlendModeCache() const {

	sortedObject3Ds_.clear();

	for (auto& [id, obj] : object3Ds_) {

		const auto& renderingData = obj.model->renderingData;
		// 描画無効
		if (!renderingData.drawEnable) {
			continue;
		}
		sortedObject3Ds_[renderingData.blendMode].emplace_back(&obj);
	}
	needsSorting_ = false;
}

const std::unordered_map<BlendMode, std::vector<const ComponentManager::Object3DForGPU*>>&
ComponentManager::GetSortedObject3Ds() const {

	if (needsSorting_) {
		RebuildBlendModeCache();
	}
	return sortedObject3Ds_;
}

void ComponentManager::SelectObject() {

	componentImGui_->SelectObject3D();
}

void ComponentManager::EditObject() {

	componentImGui_->EditObject3D();
}