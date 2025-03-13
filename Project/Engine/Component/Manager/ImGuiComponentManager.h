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
class RenderObjectManager;

// entityID
using EntityID = uint32_t;

//============================================================================
//	ImGuiComponentManager class
//============================================================================
class ImGuiComponentManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ImGuiComponentManager() = default;
	~ImGuiComponentManager() = default;

	void Init(EntityManager* entityManager, Transform3DManager* transform3DManager,
		MaterialManager* materialManager, ModelComponentManager* modelComponentManager,
		RenderObjectManager* renderObjectManager);

	// Object3Dの選択
	void SelectObject3D();
	// 選択したObject3Dの操作
	void EditObject3D();

	//--------- accessor -----------------------------------------------------

	void SetImGuiFunc(EntityID entityId, std::function<void()> func);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct EditImGui {

		// imguiで選択されたidの保持
		std::optional<uint32_t> selectedId_ = std::nullopt;
		std::unordered_map<EntityID, std::function<void()>> imguiFunc_;
		mutable std::unordered_map<std::string, std::vector<std::pair<int, uint32_t>>> grouped_;
		mutable size_t lastCount_ = 0;
	};

	//--------- variables ----------------------------------------------------

	EntityManager* entityManager_;
	Transform3DManager* transform3DManager_;
	MaterialManager* materialManager_;
	ModelComponentManager* modelComponentManager_;
	RenderObjectManager* renderObjectManager_;

	const float itemWidth_ = 168.0f;

	EditImGui object3D_;
	int selectedMaterialIndex_ = 0;

	//--------- functions ----------------------------------------------------

	// Object詳細、操作
	void Object3DInformation();
	void Object3DRenderingData();
	void Object3DTransform();
	void Object3DMaterial();
};