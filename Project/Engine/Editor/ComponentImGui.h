#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <unordered_map>
#include <algorithm>
#include <functional>
// front
class EntityManager;
class Transform3DManager;
class MaterialManager;
class ModelComponentManager;

// entityID
using EntityID = uint32_t;

//============================================================================
//	ComponentImGui class
//============================================================================
class ComponentImGui {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ComponentImGui() = default;
	~ComponentImGui() = default;

	void Init(EntityManager* entityManager, Transform3DManager* transform3DManager,
		MaterialManager* materialManager, ModelComponentManager* modelComponentManager);

	std::function<void()> AddComponent(EntityID id);
	void RemoveComponent(EntityID id);

	// Object3Dの選択
	void SelectObject3D();
	// 選択したObject3Dの操作
	void EditObject3D();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct EditImGui {

		// imguiで選択されたidの保持
		std::optional<uint32_t> selectedId_ = std::nullopt;
		mutable std::unordered_map<std::string, std::vector<std::pair<int, uint32_t>>> grouped_;
		mutable size_t lastCount_ = 0;
	};

	//--------- variables ----------------------------------------------------

	EntityManager* entityManager_;
	Transform3DManager* transform3DManager_;
	MaterialManager* materialManager_;
	ModelComponentManager* modelComponentManager_;

	std::unordered_map<EntityID, std::function<void()>> object3DImGui_;

	EditImGui object3D_;
	int selectedMaterialIndex_ = 0;

	//--------- functions ----------------------------------------------------

	// Object詳細、操作
	void Object3DInformation();
	void Object3DRenderingData();
	void Object3DTransform();
	void Object3DMaterial();
};