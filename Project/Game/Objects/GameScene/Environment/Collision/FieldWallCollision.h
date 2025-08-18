#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/Collider.h>

// front(押し戻し対象)
class Player;
class BossEnemy;

//============================================================================
//	FieldWallCollision class
//============================================================================
class FieldWallCollision :
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FieldWallCollision() = default;
	~FieldWallCollision() = default;

	void Init();

	void Update();

	/*-------- collision ----------*/

	// 衝突コールバック関数
	void OnCollisionStay(const CollisionBody* collisionBody) override;

	// editor
	void ImGui(uint32_t index);

	// json
	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	void SetPushBackTarget(Player* player, BossEnemy* bossEnemy);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 押し戻し対象
	Player* player_;
	BossEnemy* bossEnemy_;

	//--------- functions ----------------------------------------------------

	CollisionShape::AABB GetWorldAABB() const;
	CollisionShape::AABB MakeAABBProxy(const CollisionBody* other);
	Vector3 ComputePushVector(const CollisionShape::AABB& wall, const CollisionShape::AABB& actor);
};