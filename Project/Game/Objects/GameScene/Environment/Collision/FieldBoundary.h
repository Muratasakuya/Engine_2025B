#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Objects/GameScene/Environment/Collision/FieldWallCollision.h>

//============================================================================
//	FieldBoundary class
//============================================================================
class FieldBoundary :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FieldBoundary() :IGameEditor("FieldWallCollisionCollection") {}
	~FieldBoundary() = default;

	void Init();

	// 制御処理
	void ControlTargetMove();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	void SetPushBackTarget(Player* player, BossEnemy* bossEnemy);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Player* player_;
	BossEnemy* bossEnemy_;

	// AABBの押し戻し領域
	std::vector<std::unique_ptr<FieldWallCollision>> collisions_;
	// 座標移動制限
	float moveClampLength_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateAllCollisionBody();
};