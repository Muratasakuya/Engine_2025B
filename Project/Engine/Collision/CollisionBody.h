#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionGeometry.h>

// c++
#include <cstdint>
#include <optional>
#include <functional>

//============================================================================
//	ColliderType
//============================================================================

enum class ColliderType {

	Type_None = 0, // ビットが立っていない状態
	Type_Test = 1 << 0,
	Type_Player = 1 << 1,
	Type_BossEnemy = 1 << 2,
	Type_CrossMarkWall = 1 << 3,
};

// operator
inline ColliderType operator|(ColliderType lhs, ColliderType rhs) {
	using T = std::underlying_type_t<ColliderType>;
	return static_cast<ColliderType>(static_cast<T>(lhs) | static_cast<T>(rhs));
}
inline ColliderType& operator|=(ColliderType& lhs, ColliderType rhs) {
	lhs = lhs | rhs;
	return lhs;
}
inline ColliderType operator&(ColliderType lhs, ColliderType rhs) {
	using T = std::underlying_type_t<ColliderType>;
	return static_cast<ColliderType>(static_cast<T>(lhs) & static_cast<T>(rhs));
}
inline ColliderType& operator&=(ColliderType& lhs, ColliderType rhs) {
	lhs = lhs & rhs;
	return lhs;
}

//============================================================================
//	CollisionBody  class
//============================================================================
class CollisionBody {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	CollisionBody() = default;
	~CollisionBody() = default;

	void TriggerOnCollisionEnter(CollisionBody* collider);

	void TriggerOnCollisionStay(CollisionBody* collider);

	void TriggerOnCollisionExit(CollisionBody* collider);

	void SetOnCollisionEnter(std::function<void(CollisionBody*)> onCollisionEnter) { onEnter_ = onCollisionEnter; }
	void SetOnCollisionStay(std::function<void(CollisionBody*)> onCollisionEnter) { onStay_ = onCollisionEnter; }
	void SetOnCollisionExit(std::function<void(CollisionBody*)> onCollisionEnter) { onExit_ = onCollisionEnter; }

	void UpdateSphere(const CollisionShape::Sphere& sphere);
	void UpdateAABB(const CollisionShape::AABB& aabb);
	void UpdateOBB(const CollisionShape::OBB& obb);

	//--------- accessor -----------------------------------------------------

	void SetShape(const CollisionShape::Shapes& shape) { shape_ = shape; }

	void SetType(ColliderType type) { type_ = type; }
	void SetTargetType(ColliderType target) { targetType_ = target; }

	ColliderType GetType() const { return type_; }
	ColliderType GetTargetType() const { return targetType_; }

	const CollisionShape::Shapes& GetShape() const { return shape_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- using --------------------------------------------------------

	using CollisionCallback = std::function<void(CollisionBody*)>;

	//--------- variables ----------------------------------------------------

	// コールバック関数
	CollisionCallback onEnter_ = nullptr;
	CollisionCallback onStay_ = nullptr;
	CollisionCallback onExit_ = nullptr;

	ColliderType type_;       // 自身の衝突タイプ
	ColliderType targetType_; // 衝突相手のタイプ

	CollisionShape::Shapes shape_; // 衝突判定を行う形状
};