#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/Manager/EntityManager.h>
#include <Engine/Component/Manager/Transform3DManager.h>
#include <Engine/Component/Manager/MaterialManager.h>
#include <Engine/Component/Manager/ModelComponentManager.h>
#include <Engine/Editor/ComponentImGui.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>
#include <variant>
#include <unordered_map>
// front
class Asset;
class SRVManager;

// 今現段階で必要と思った種類
// 3DObjectを扱いたいとき
// Transform3D、Materials、Model(Animationするかで分岐)、Rendering情報(BlendModeなど)、Collisionは複数設定したい、Physics
// これらはすべて同じIDで作成したい
// AddComponnentしたら引き数でModelName、Animationするかしないか、ObjectNameを受け取り作成し、ユーザーに操作できるデータをポインタで渡す
// Spriteを扱いたいとき
// Transform2D、Color、Rendering情報(BlendModeなど)、Collision
// これらはすべて同じIDで作成したい
// AddComponnentしたら引き数でTextureName、ObjectNameを受け取り作成し、ユーザーに操作できるデータをポインタで渡す

//============================================================================
//	ComponentManager class
//============================================================================
class ComponentManager {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structures ---------------------------------------------------

	struct Object3DForGPU {

		DxConstBuffer<TransformationMatrix> matrix;
		std::vector<DxConstBuffer<Material>> materials;
		ModelComponent* model;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ComponentManager() = default;
	~ComponentManager() = default;

	void Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager);
	void Update();

	Object3D CreateObject3D(const std::string& modelName,
		const std::optional<std::string>& animationName, const std::string& objectName);

	void RemoveObject3D(EntityID id);

	void InvalidateBlendModeCache();

	// Object選択
	void SelectObject();
	// 選択したObject操作
	void EditObject();

	//--------- accessor -----------------------------------------------------

	const std::unordered_map<BlendMode, std::vector<const Object3DForGPU*>>&
		GetSortedObject3Ds() const;

	// singleton
	static ComponentManager* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ComponentManager* instance_;

	ID3D12Device* device_;
	Asset* asset_;
	SRVManager* srvManager_;

	std::unordered_map<EntityID, Object3DForGPU> object3Ds_;

	// blendModeごとのObject
	mutable bool needsSorting_ = true;
	mutable std::unordered_map<BlendMode, std::vector<const Object3DForGPU*>> sortedObject3Ds_;

	// components
	std::unique_ptr<EntityManager> entityManager_;

	std::unique_ptr<Transform3DManager> transform3DManager_;

	std::unique_ptr<MaterialManager> materialManager_;

	std::unique_ptr<ModelComponentManager> modelComponentManager_;

	// imgui
	std::unique_ptr<ComponentImGui> componentImGui_;

	//--------- functions ----------------------------------------------------

	void RebuildBlendModeCache() const;
};