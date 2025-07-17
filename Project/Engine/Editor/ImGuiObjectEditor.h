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
class TagSystem;
class ObjectManager;
class IGameObject;

//============================================================================
//	ImGuiObjectEditor class
//============================================================================
class ImGuiObjectEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init();

	// objectの選択
	void SelectObject();
	// 選択したobjectの操作
	void EditObject();

	// 選択全解除
	void Reset();

	void Registerobject(uint32_t id, IGameObject* object);

	//--------- accessor -----------------------------------------------------

	// singleton
	static ImGuiObjectEditor* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct EditImGui {

		// imguiで選択されたidの保持
		std::optional<uint32_t> selectedId_ = std::nullopt;
		std::unordered_map<uint32_t, std::function<void()>> imguiFunc_;
	};

	//--------- variables ----------------------------------------------------

	static ImGuiObjectEditor* instance_;

	TagSystem* tagSystem_;
	ObjectManager* ObjectManager_;

	std::unordered_map<std::string, std::vector<uint32_t>> groups_;

	std::unordered_map<uint32_t, std::optional<IGameObject*>> objectsMap_;

	std::optional<uint32_t> selected3D_;
	int selectedMaterialIndex_ = 0;

	std::optional<uint32_t> selected2D_;
	int  selectedSpriteIndex_ = 0;

	const float itemWidth_ = 224.0f;

	//--------- functions ----------------------------------------------------

	ImGuiObjectEditor() = default;
	~ImGuiObjectEditor() = default;
	ImGuiObjectEditor(const ImGuiObjectEditor&) = delete;
	ImGuiObjectEditor& operator=(const ImGuiObjectEditor&) = delete;

	bool Is3D(uint32_t object) const;
	bool Is2D(uint32_t object) const;
	void DrawSelectable(uint32_t object, const std::string& name);

	//----------- group ------------------------------------------------------

	// groupの作成
	void CreateGroup();
	// group化されたobjectの選択
	void SelectGroupedObject();

	//--------- Objects -----------------------------------------------------

	// Object詳細、操作
	void EditObjects();
	void ObjectsInformation();
	void ObjectsTransform();
	void ObjectsMaterial();

	void EditSkybox();

	//--------- object2D -----------------------------------------------------

	void EditObject2D();
	void Object2DInformation();
	void Object2DSprite();
	void Object2DTransform();
	void Object2DMaterial();
};