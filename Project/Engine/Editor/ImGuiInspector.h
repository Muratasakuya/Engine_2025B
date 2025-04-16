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
class EntityRegistry;

//============================================================================
//	ImGuiInspector class
//============================================================================
class ImGuiInspector {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	// objectの選択
	void SelectObject();
	// 選択したobjectの操作
	void EditObject();

	// 選択全解除
	void Reset();

	//--------- accessor -----------------------------------------------------

	// singleton
	static ImGuiInspector* GetInstance();
	static void Finalize();

	void SetEntityManager(
		EntityRegistry* entity3DRegistry,
		EntityRegistry* effectRegistry,
		EntityRegistry* entity2DRegistry);

	void SetImGuiFunc(uint32_t entityId, std::function<void()> func);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- enum class ---------------------------------------------------

	enum class EntityType {

		Object3D,
		Effect,
		Object2D,
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

	static ImGuiInspector* instance_;

	EntityRegistry* entity3DRegistry_;
	EntityRegistry* effectRegistry_;
	EntityRegistry* entity2DRegistry_;

	const float itemWidth_ = 192.0f;

	std::unordered_map<std::string, std::vector<EntityReference>> groupedEntities_;
	std::vector<uint32_t> prevEntity3DIds_;
	std::vector<uint32_t> prevEffectIds_;
	std::vector<uint32_t> prevEntity2DIds_;

	EditImGui object3D_;
	int selectedMaterialIndex_ = 0;

	EditImGui effect_;

	EditImGui object2D_;

	//--------- functions ----------------------------------------------------

	ImGuiInspector() = default;
	~ImGuiInspector() = default;
	ImGuiInspector(const ImGuiInspector&) = delete;
	ImGuiInspector& operator=(const ImGuiInspector&) = delete;

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

	//--------- Effect -------------------------------------------------------

	// Effect詳細、操作
	void EditEffect();
	void EffectInformation();
	void EffectMesh();
	void EffectTransform();
	void EffectMaterial();

	//--------- object2D -----------------------------------------------------

	// Object詳細、操作
	void EditObject2D();
	void Object2DInformation();
	void Object2DRenderingData();
	void Object2DTransform();
	void Object2DMaterial();
};