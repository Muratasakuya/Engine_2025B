#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/Collider.h>

// front(押し戻し対象)
class Player;
class BossEnemy;

//============================================================================
//	FieldCollision class
//============================================================================
class FieldCollision :
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FieldCollision() = default;
	~FieldCollision() = default;

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
	void SetTranslation(const Vector3& translation) { translation_ = translation; }

	const Vector3& GetTranslation() const { return translation_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 押し戻し対象
	Player* player_;
	BossEnemy* bossEnemy_;

	// 中心座標
	Vector3 translation_;

	//--------- functions ----------------------------------------------------

	CollisionShape::AABB GetWorldAABB() const;
	CollisionShape::AABB MakeAABBProxy(const CollisionBody* other);
	Vector3 ComputePushVector(const CollisionShape::AABB& wall, const CollisionShape::AABB& actor);
};