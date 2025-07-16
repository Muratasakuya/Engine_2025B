#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/Collider.h>
#include <Game/Objects/Enemy/Boss/Structures/BossEnemyStructures.h>

//============================================================================
//	BossEnemyAttackCollision class
//============================================================================
class BossEnemyAttackCollision :
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyAttackCollision() = default;
	~BossEnemyAttackCollision() = default;

	void Init();

	void Update(const Transform3DComponent& transform);

	void ImGui();

	// json
	void ApplyJson(const Json& data);
	void SaveJson(Json& data);

	// 衝突コールバック関数
	void OnCollisionEnter(const CollisionBody* collisionBody) override;

	//--------- accessor -----------------------------------------------------

	void SetEnterState(BossEnemyState state);

	bool CanParry() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 判定区間
	struct TimeWindow {

		float on;  // 衝突開始時間(判定入り)
		float off; // 衝突終了時間(判定を消す)
	};

	struct AttackParameter {

		Vector3 centerOffset; // player座標からのオフセット
		Vector3 size;         // サイズ

		std::vector<TimeWindow> windows;
		std::vector<TimeWindow> parryWindows;
		bool isParryPossible = false;
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	BossEnemyState currentState_;

	CollisionBody* weaponBody_;

	std::unordered_map<BossEnemyState, AttackParameter> table_; // 状態毎の衝突

	float currentTimer_; // 現在の経過時間、全部共通

	// editor
	int editingIndex_;

	//--------- functions ----------------------------------------------------

	// helper
	BossEnemyState GetBossEnemyStateFromName(const std::string& name);
	void EditWindowParameter(const std::string& label, std::vector<TimeWindow>& windows);
};