#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Objects/GameScene/Environment/Collision/FieldCollision.h>

//============================================================================
//	FieldCollisionCollection class
//============================================================================
class FieldCollisionCollection :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FieldCollisionCollection() :IGameEditor("FieldCollisionCollection") {}
	~FieldCollisionCollection() = default;

	void Init();

	void Update();

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

	std::vector<std::unique_ptr<FieldCollision>> collisions_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();
};