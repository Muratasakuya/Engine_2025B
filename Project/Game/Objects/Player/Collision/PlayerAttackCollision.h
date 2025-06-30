#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/Collider.h>
#include <Game/Objects/Player/Structures/PlayerStructures.h>

//============================================================================
//	PlayerAttackCollision class
//============================================================================
class PlayerAttackCollision :
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttackCollision() = default;
	~PlayerAttackCollision() = default;

	void Init();

	void Update(const Transform3DComponent& transform);

	void ImGui();

	// json
	void ApplyJson(const Json& data);
	void SaveJson(Json& data);

	// 衝突コールバック関数
	void OnCollisionEnter(const CollisionBody* collisionBody) override;

	//--------- accessor -----------------------------------------------------

	void SetEnterState(PlayerState state);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct AttackParameter {

		Vector3 centerOffset; // player座標からのオフセット
		Vector3 size;         // サイズ
		float onTime;         // 衝突開始時間(判定入り)
		float offTime;        // 衝突終了時間(判定を消す)
		float hitInterval;    // 多段ヒット
	};

	//--------- variables ----------------------------------------------------

	CollisionBody* weaponBody_;
	const Transform3DComponent* transform_;

	std::unordered_map<PlayerState, AttackParameter> table_; // 状態毎の衝突
	const AttackParameter* currentParameter_;                // 現在の状態の値

	float currentTimer_; // 現在の経過時間、全部共通
	float reHitTimer_;   // 多段ヒット経過時間

	// editor
	int editingIndex_;

	//--------- functions ----------------------------------------------------

	// helper
	PlayerState GetPlayerStateFromName(const std::string& name);
};