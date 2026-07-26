#include "ObjectManager.h"

using namespace SakuEngine;

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Editor/GameObject/ImGuiObjectEditor.h>

// data
#include <Engine/Object/Data/Transform/Transform.h>
#include <Engine/Object/Data/Material/Material.h>
#include <Engine/Object/Data/Skinned/SkinnedAnimation.h>
#include <Engine/Object/Data/Tag/ObjectTag.h>
#include <Engine/Object/Data/Render/MeshRender.h>
// systems
#include <Engine/Object/System/Systems/TransformSystem.h>
#include <Engine/Object/System/Systems/MaterialSystem.h>
#include <Engine/Object/System/Systems/AnimationSystem.h>
#include <Engine/Object/System/Systems/InstancedMeshSystem.h>
#include <Engine/Object/System/Systems/SpriteBufferSystem.h>
#include <Engine/Object/System/Systems/MSDFTextBufferSystem.h>
#include <Engine/Object/System/Systems/SkyboxRenderSystem.h>
#include <Engine/Object/System/Systems/CanvasBufferSystem.h>
#include <Engine/Object/System/Systems/TagSystem.h>

//============================================================================
//	ObjectManager classMethods
//============================================================================

ObjectManager* ObjectManager::instance_ = nullptr;

/// <summary>
/// ObjectManagerのシングルトンインスタンスを取得する。
/// </summary>
/// <returns>ObjectManagerの共有インスタンス。</returns>
ObjectManager* ObjectManager::GetInstance() {

	static ObjectManager instance;
	instance_ = &instance;
	return instance_;
}

/// <summary>
/// ObjectManagerのシングルトン参照を無効化する。
/// </summary>
void ObjectManager::Finalize() {

	if (instance_ != nullptr) {

		instance_ = nullptr;
	}
}

/// <summary>
/// デバイス、アセット、コマンドを受け取り、各Systemとオブジェクトプールを初期化する。
/// </summary>
/// <param name="device">GPUリソース作成に使用するDirect3D12デバイス。</param>
/// <param name="asset">モデル、テクスチャ、フォントなどを取得するアセット管理。</param>
/// <param name="dxCommand">描画バッファ構築に使用するコマンド制御。</param>
void ObjectManager::Init(ID3D12Device* device, Asset* asset, DxCommand* dxCommand) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	objectPoolManager_ = std::make_unique<ObjectPoolManager>();
	systemManager_ = std::make_unique<SystemManager>();

	// system登録
	systemManager_->AddSystem<Transform3DSystem>();
	systemManager_->AddSystem<Transform2DSystem>();
	systemManager_->AddSystem<TextTransform2DSystem>();
	systemManager_->AddSystem<AnimationSystem>();
	systemManager_->AddSystem<MaterialSystem>();
	systemManager_->AddSystem<SpriteMaterialSystem>();
	systemManager_->AddSystem<TextMaterialSystem>();
	systemManager_->AddSystem<TagSystem>();
	systemManager_->AddSystem<SpriteBufferSystem>();
	systemManager_->AddSystem<MSDFTextBufferSystem>();
	systemManager_->AddSystem<SkyboxRenderSystem>();
	systemManager_->AddSystem<CanvasBufferSystem>();
	systemManager_->AddSystem<InstancedMeshSystem>(device, asset, dxCommand);
	systemManager_->GetSystem<InstancedMeshSystem>()->StartBuildWorker();

	ImGuiObjectEditor::GetInstance()->Init();
}

/// <summary>
/// 登録済みSystemのデータ更新処理を実行する。
/// </summary>
void ObjectManager::UpdateData() {

	systemManager_->UpdateData(*objectPoolManager_.get());
}

/// <summary>
/// 登録済みSystemのGPUバッファ更新処理を実行する。
/// </summary>
void ObjectManager::UpdateBuffer() {

	systemManager_->UpdateBuffer(*objectPoolManager_.get());
}

/// <summary>
/// モデル名をもとにTransform、Material、MeshRender、必要ならAnimationを持つ3Dオブジェクトを作成する。
/// </summary>
/// <param name="modelName">描画に使用するロード済みモデル名。</param>
/// <param name="name">作成するオブジェクトの識別名。</param>
/// <param name="groupName">オブジェクトを所属させるグループ名。</param>
/// <param name="animationName">スキニング用に関連付けるアニメーション名。未指定なら静的メッシュとして扱う。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::CreateObjects(const std::string& modelName,
	const std::string& name, const std::string& groupName,
	const std::optional<std::string>& animationName) {

	LOG_SCOPE_MS_LABEL(modelName);

	// まず共通のObjectTagを持つ空オブジェクトを作成し、以降で3D描画に必要なデータを追加する。
	uint32_t object = BuildEmptyObject(name, groupName);
	// 必要なdataを作成
	auto* transform = objectPoolManager_->AddData<SakuEngine::Transform3D>(object);
	auto* materialsPtr = objectPoolManager_->AddData<Material, true>(object);
	auto* meshRender = objectPoolManager_->AddData<MeshRender>(object);

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

	// meshRender
	meshRender->Init(modelName);

	if (animationName.has_value()) {

		// animation処理がある場合はdataを追加
		auto* animation = objectPoolManager_->AddData<SkinnedAnimation>(object);
		// 初期化
		animation->Init(*animationName, asset_);

		LOG_INFO("created object3D: name: [{}] skinnedMesh: [{}] animation: [{}]", name, modelName, animationName.value());
	} else {

		LOG_INFO("created object3D: name: [{}] staticMesh: [{}]", name, modelName);
	}
	return object;
}

/// <summary>
/// 指定テクスチャを使用するスカイボックスオブジェクトを作成する。
/// </summary>
/// <param name="textureName">スカイボックス描画に使用するキューブマップテクスチャ名。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::CreateSkybox(const std::string& textureName) {

	LOG_SCOPE_MS_LABEL("skybox");

	// object作成
	uint32_t object = BuildEmptyObject("skybox", "Environment");
	// 必要なdataを作成
	auto* skybox = objectPoolManager_->AddData<Skybox>(object);

	// dataを初期化
	skybox->Create(device_, asset_->GetTextureGPUIndex(textureName), object);
	LOG_INFO("created skybox: textureName: [{}]", textureName);

	return object;
}

/// <summary>
/// エフェクト用Transformを持つ空のエフェクトオブジェクトを作成する。
/// </summary>
/// <param name="name">作成するエフェクトオブジェクトの識別名。</param>
/// <param name="groupName">オブジェクトを所属させるグループ名。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::CreateEffect(const std::string& name, const std::string& groupName) {

	LOG_SCOPE_MS_LABEL("effect");

	// object作成
	uint32_t object = BuildEmptyObject(name, groupName);
	// 必要なdataを作成
	auto* transform = objectPoolManager_->AddData<EffectTransform>(object);

	// dataを初期化
	transform->Init();
	LOG_INFO("created effect: name: [{}]", name);

	return object;
}

/// <summary>
/// 指定テクスチャを使用する2Dスプライトオブジェクトを作成する。
/// </summary>
/// <param name="textureName">スプライト描画に使用するテクスチャ名。</param>
/// <param name="name">作成するオブジェクトの識別名。</param>
/// <param name="groupName">オブジェクトを所属させるグループ名。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::CreateObject2D(const std::string& textureName,
	const std::string& name, const std::string& groupName) {

	LOG_SCOPE_MS_LABEL(textureName);

	// object作成
	uint32_t object = BuildEmptyObject(name, groupName);
	// 必要なdataを作成
	auto* transform = objectPoolManager_->AddData<Transform2D>(object);
	auto* material = objectPoolManager_->AddData<SpriteMaterial>(object);

	// 各dataを初期化
	// transform
	transform->Init(device_);
	// material
	material->Init(device_);
	// sprite
	Sprite* sprite = objectPoolManager_->AddData<Sprite>(object, device_, asset_, textureName, *transform);
	sprite->SetRenderResources(object);

	LOG_INFO("created object2D: name: [{}] textureName: [{}]", name, textureName);

	return object;
}

/// <summary>
/// MSDFフォントとアトラステクスチャを使用するテキストオブジェクトを作成する。
/// </summary>
/// <param name="atlasTextureName">文字描画に使用するMSDFアトラステクスチャ名。</param>
/// <param name="fontJsonPath">フォントメトリクスを読み込むJSONファイルパス。</param>
/// <param name="name">作成するオブジェクトの識別名。</param>
/// <param name="groupName">オブジェクトを所属させるグループ名。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::CreateTextObject(const std::string& atlasTextureName,
	const std::string& fontJsonPath, const std::string& name, const std::string& groupName) {

	LOG_SCOPE_MS_LABEL(atlasTextureName);

	// object作成
	uint32_t object = BuildEmptyObject(name, groupName);
	// 必要なdataを作成
	auto* transform = objectPoolManager_->AddData<TextTransform2D>(object);
	auto* material = objectPoolManager_->AddData<MSDFTextMaterial>(object);
	constexpr const uint32_t maxGlyphCount = 128;
	auto* font = GetSystem<MSDFTextBufferSystem>()->GetMSDFFont(asset_, atlasTextureName, fontJsonPath);
	MSDFText* text = objectPoolManager_->AddData<MSDFText>(object, device_, asset_, font, maxGlyphCount);

	// 各dataを初期化
	// transform
	transform->Init(device_);
	transform->Secure(device_, maxGlyphCount);
	// material
	material->Init(device_);
	// 初期設定
	material->material.atlasSize = font->GetAtlasSize();
	material->material.pixelRange = font->GetPxRange();
	// text
	text->SetRenderResources(object);

	LOG_INFO("created textObject: name: [{}] textureName: [{}]", name, atlasTextureName);

	return object;
}

/// <summary>
/// ObjectTagだけを持つ空オブジェクトを作成し、後続処理で必要なデータを追加できる状態にする。
/// </summary>
/// <param name="name">作成するオブジェクトの識別名。</param>
/// <param name="groupName">オブジェクトを所属させるグループ名。</param>
/// <returns>作成されたオブジェクトID。</returns>
uint32_t ObjectManager::BuildEmptyObject(const std::string& name, const std::string& groupName) {

	// object作成
	uint32_t object = objectPoolManager_->Create();
	// tag設定
	auto* tag = objectPoolManager_->AddData<ObjectTag>(object);
	tag->identifier = name;
	tag->name = systemManager_->GetSystem<TagSystem>()->CheckName(name);
	tag->groupName = groupName;
	tag->objectID = object;

	return object;
}

/// <summary>
/// 指定オブジェクトIDに紐づく全データを破棄対象にする。
/// </summary>
/// <param name="object">破棄対象のオブジェクトID。</param>
void ObjectManager::Destroy(uint32_t object) {

	objectPoolManager_->Destroy(object);
}

/// <summary>
/// destroyOnLoadが有効な全オブジェクトを一括破棄する。
/// </summary>
void ObjectManager::DestroyAll() {

	// すべて走査して破棄
	Archetype mask{};
	auto objects = objectPoolManager_->View(mask);
	for (uint32_t id : objects) {

		// 破棄フラグがたっていなければ破棄しない
		const auto& tag = objectPoolManager_->GetData<ObjectTag>(id);
		if (!tag->destroyOnLoad) {
			continue;
		}
		objectPoolManager_->Destroy(id);
	}
}
