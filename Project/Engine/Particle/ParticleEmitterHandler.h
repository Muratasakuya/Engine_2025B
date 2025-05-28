#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector3.h>

// imgui
#include <imgui.h>
// c++
#include <string>
#include <vector>
#include <optional>

//============================================================================
//	ParticleEmitterHandler class
//============================================================================
class ParticleEmitterHandler {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleEmitterHandler() = default;
	~ParticleEmitterHandler() = default;

	void Init();

	void ImGui(float itemWidth);

	void ClearNotification();
	//--------- accessor -----------------------------------------------------

	bool IsAddEmitter() const { return addEmitter_; }
	const std::optional<Json>& GetLoadEmitterData() const { return loadEmitterData_; }
	const std::string& GetAddEmitterName() const { return emitterNames_.back(); }

	const std::optional<std::string>& GetSelectEmitterName() const { return selectEmitterName_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structrue ----------------------------------------------------

	struct InputTextValue {

		char nameBuffer[128]; // imgui入力用
		std::string name;     // 入力した文字を取得する用

		void Reset();
	};

	//--------- variables ----------------------------------------------------

	// 追加通知用
	bool addEmitter_;
	// 読み込んで作成するemitterかどうか
	std::optional<Json> loadEmitterData_;

	// emitter追加入力処理
	InputTextValue addInputText_;
	// 選択されたemitterの名前
	std::optional<std::string> selectEmitterName_;

	// 追加されたemitterの名前list
	std::vector<std::string> emitterNames_;
	int currentSelectIndex_;

	// editor
	ImVec2 addButtonSize_;  // 追加ボタンサイズ
	ImVec2 leftChildSize_;  // 左側
	ImVec2 rightChildSize_; // 右側

	//--------- functions ----------------------------------------------------

	void EditLayout();

	// 追加処理
	void AddEmitter();
	// 読み込み処理
	void LoadEmitter();
	// 選択処理
	void SelectEmitter();
};