#include "ObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Editor/ImGuiObjectEditor.h>

// data
#include <Engine/Object/Data/Transform.h>
#include <Engine/Object/Data/Material.h>
#include <Engine/Object/Data/Animation.h>
#include <Engine/Object/Data/ObjectTag.h>
// systems
#include <Engine/Object/System/Systems/TransformSystem.h>
#include <Engine/Object/System/Systems/MaterialSystem.h>
#include <Engine/Object/System/Systems/AnimationSystem.h>
#include <Engine/Object/System/Systems/InstancedMeshSystem.h>
#include <Engine/Object/System/Systems/SpriteBufferSystem.h>
#include <Engine/Object/System/Systems/SkyboxRenderSystem.h>
#include <Engine/Object/System/Systems/TagSystem.h>

//============================================================================
//	ObjectManager classMethods
//============================================================================

ObjectManager* ObjectManager::instance_ = nullptr;

ObjectManager* ObjectManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ObjectManager();
	}
	return instance_;
}

void ObjectManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ObjectManager::Init(ID3D12Device* device, Asset* asset, DxCommand* dxCommand) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	ObjectPoolManager_ = std::make_unique<ObjectPoolManager>();
	systemManager_ = std::make_unique<SystemManager>();

	// system登録
	systemManager_->AddSystem<Transform3DSystem>();
	systemManager_->AddSystem<Transform2DSystem>();
	systemManager_->AddSystem<AnimationSystem>();
	systemManager_->AddSystem<MaterialSystem>();
	systemManager_->AddSystem<SpriteMaterialSystem>();
	systemManager_->AddSystem<TagSystem>();
	systemManager_->AddSystem<InstancedMeshSystem>(device, asset, dxCommand);
	systemManager_->AddSystem<SpriteBufferSystem>();
	systemManager_->AddSystem<SkyboxRenderSystem>();

	ImGuiObjectEditor::GetInstance()->Init();
}

void ObjectManager::UpdateData() {

	systemManager_->UpdateData(*ObjectPoolManager_.get());
}

void ObjectManager::UpdateBuffer() {

	systemManager_->UpdateBuffer(*ObjectPoolManager_.get());
}

uint32_t ObjectManager::CreateObjects(const std::string& modelName,
	const std::string& name, const std::string& groupName,
	const std::optional<std::string>& animationName) {

	// object作成
	uint32_t object = BuildEmptyobject(name, groupName);
	// 必要なdataを作成
	auto* transform = ObjectPoolManager_->AddData<Transform3D>(object);
	auto* materialsPtr = ObjectPoolManager_->AddData<Material, true>(object);

	// 各dataを初期化
	// transform
	transform->Init();
	// instancingのデータ名を設定
	transform->SetInstancingName(modelName);

	// material
	const ModelData& modelData = asset_->GetModelData(modelName);
	auto& materials = *materialsPtr;
	systemManager_->GetSystem<MaterialSystem>()->Init(
		materials, modelData, asset_);

	if (animationName.has_value()) {

		// animation処理がある場合はdataを追加
		auto* animation = ObjectPoolManager_->AddData<SkinnedAnimation>(object);
		// 初期化
		animation->Init(*animationName, asset_);

		// bufferを作成
		systemManager_->GetSystem<InstancedMeshSystem>()->CreateSkinnedMesh(modelName);
	} else {

		// bufferを作成
		systemManager_->GetSystem<InstancedMeshSystem>()->CreateStaticMesh(modelName);
	}

	return object;
}

uint32_t ObjectManager::CreateSkybox(const std::string& textureName) {

	// object作成
	uint32_t object = BuildEmptyobject("skybox", "Environment");
	// 必要なdataを作成
	auto* skybox = ObjectPoolManager_->AddData<Skybox>(object);

	// dataを初期化
	skybox->Create(device_, asset_->GetTextureGPUIndex(textureName));

	return object;
}

uint32_t ObjectManager::CreateObject2D(const std::string& textureName,
	const std::string& name, const std::string& groupName) {

	// object作成
	uint32_t object = BuildEmptyobject(name, groupName);
	// 必要なdataを作成
	auto* transform = ObjectPoolManager_->AddData<Transform2D>(object);
	auto* material = ObjectPoolManager_->AddData<SpriteMaterial>(object);

	// 各dataを初期化
	// transform
	transform->Init(device_);
	// material
	material->Init(device_);
	// sprite
	ObjectPoolManager_->AddData<Sprite>(object,
		device_, asset_, textureName, *transform);

	return object;
}

void ObjectManager::Destroy(uint32_t object) {

	ObjectPoolManager_->Destroy(object);
}

uint32_t ObjectManager::BuildEmptyobject(const std::string& name, const std::string& groupName) {

	// object作成
	uint32_t object = ObjectPoolManager_->Create();
	// tag設定
	auto* tag = ObjectPoolManager_->AddData<ObjectTag>(object);
	tag->name = systemManager_->GetSystem<TagSystem>()->CheckName(name);
	tag->groupName = groupName;

	return object;
}