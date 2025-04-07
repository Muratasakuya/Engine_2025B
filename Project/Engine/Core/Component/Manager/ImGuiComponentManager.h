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
// 3D
class Transform3DManager;
class MaterialManager;
class ModelComponentManager;
// 2D
class Transform2DManager;
class SpriteMaterialManager;
class SpriteComponentManager;

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

	void Init(
		// 3D
		EntityManager* entity3DManager, Transform3DManager* transform3DManager,
		MaterialManager* materialManager,
		// 2D
		EntityManager* entity2DManager, Transform2DManager* transform2DManager,
		SpriteMaterialManager* spriteMaterialManager, SpriteComponentManager* spriteComponentManager);

	// objectの選択
	void SelectObject();
	// 選択したobjectの操作
	void EditObject();

	// 選択全解除
	void Reset();

	//--------- accessor -----------------------------------------------------

	void SetImGuiFunc(uint32_t entityId, std::function<void()> func);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- enum class ---------------------------------------------------

	enum class EntityType {

		Object3D,
		Object2D,
		Trail,
	};

	//--------- structure ----------------------------------------------------

	struct EditImGui {

		// imguiで選択されたidの保持
		std::optional<uint32_t> selectedId_ = std::nullopt;
		std::unordered_map<uint32_t, std::function<void()>> imguiFunc_;
	};

	struct EntityReference {

		EntityType type;
		uint32_t id;
	};

	//--------- variables ----------------------------------------------------

	// 3D
	EntityManager* entity3DManager_;
	Transform3DManager* transform3DManager_;
	MaterialManager* materialManager_;
	// 2D
	EntityManager* entity2DManager_;
	Transform2DManager* transform2DManager_;
	SpriteMaterialManager* spriteMaterialManager_;
	SpriteComponentManager* spriteComponentManager_;

	const float itemWidth_ = 168.0f;

	std::unordered_map<std::string, std::vector<EntityReference>> groupedEntities_;

	EditImGui object3D_;
	int selectedMaterialIndex_ = 0;

	EditImGui object2D_;

	//--------- functions ----------------------------------------------------

	//----------- group ------------------------------------------------------

	// groupの作成
	void CreateGroup();
	// group化されたobjectの選択
	void SelectGroupedObject();
	// group化されていないobjectの選択
	void SelectUnGroupedObject();

	//--------- object3D -----------------------------------------------------

	// Object詳細、操作
	void EditObject3D();
	void Object3DInformation();
	void Object3DTransform();
	void Object3DMaterial();

	//--------- object2D -----------------------------------------------------

	// Object詳細、操作
	void EditObject2D();
	void Object2DInformation();
	void Object2DRenderingData();
	void Object2DTransform();
	void Object2DMaterial();
};