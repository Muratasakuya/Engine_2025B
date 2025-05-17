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
class ECSManager;

//============================================================================
//	ImGuiInspector class
//============================================================================
class ImGuiInspector {
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

	//--------- accessor -----------------------------------------------------

	// singleton
	static ImGuiInspector* GetInstance();
	static void Finalize();

	void SetImGuiFunc(uint32_t entity, std::function<void()> func);
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

	static ImGuiInspector* instance_;

	TagSystem* tagSystem_;
	ECSManager* ecsManager_;

	std::unordered_map<std::string, std::vector<uint32_t>> groups_;
	std::unordered_map<uint32_t, std::function<void()>> individualUI_;

	std::optional<uint32_t> selected3D_;
	int selectedMaterialIndex_ = 0;

	const float itemWidth_ = 224.0f;

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
};