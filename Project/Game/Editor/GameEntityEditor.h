#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Entity/GameEntity3D.h>

// front
class ECSManager;
class Asset;

//============================================================================
//	GameEntityEditor class
//============================================================================
class GameEntityEditor :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameEntityEditor() :IGameEditor("GameEntityEditor") {}
	~GameEntityEditor() = default;

	void Init(Asset* asset);

	void Update();

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	enum class EntityClassType {

		None, // 通常のクラス
	};

	struct EntityHandle {

		EntityClassType classType;  // どのクラスのvectorか
		size_t innerIndex;          // vector内での位置
	};

	struct InputTextValue {

		char nameBuffer[128]; // imgui入力用
		std::string name;     // 入力した文字を取得する用

		void Reset();
	};

	//--------- variables ----------------------------------------------------

	ECSManager* ecsManager_;
	Asset* asset_;

	// 3Dentityのデータ
	std::unordered_map<EntityClassType, std::vector<std::unique_ptr<GameEntity3D>>> entities_;

	// entity追加入力処理
	InputTextValue addNameInputText_;             // 名前
	std::optional<std::string> addModelName_;     // 追加するモデルの名前
	std::optional<std::string> addAnimationName_; // 追加するアニメーションの名前
	EntityClassType addClassType_;                // 追加するクラスのタイプ
	// 選択されたentityの名前
	std::optional<std::string> selectEntityName_;

	// 追加されたentityの名前list
	std::vector<std::string> entityNames_;
	std::vector<EntityHandle> entityHandles_;
	std::optional<int> currentSelectIndex_;

	// editor
	ImVec2 addButtonSize_;  // 追加ボタンサイズ
	ImVec2 leftChildSize_;  // 左側
	ImVec2 rightChildSize_; // 右側
	ImVec2 dropSize_;       // ドロップ処理受け取りサイズ

	//--------- functions ----------------------------------------------------

	void EditLayout();

	// update
	void UpdateEntities();

	// 追加処理
	void AddEntity();
	// 選択処理
	void SelectEntity();
	// 操作処理
	void EditEntity();
	// 削除処理
	void RemoveEntity();

	// helper
	void DropFile(const std::string& label, std::optional<std::string>& recieveName);

	void SelectEntityClassType(EntityClassType& classType);
};