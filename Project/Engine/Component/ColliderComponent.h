#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/Collision.h>

// c++
#include <cstdint>
#include <optional>
#include <functional>
// colliderID
using ColliderID = uint32_t;

//============================================================================
//	ColliderType
//============================================================================

enum class ColliderType {

	Type_None = 0,      // ビットが立っていない状態
	Type_Test = 1 << 0, // テスト
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
//	ColliderComponent class
//============================================================================
class ColliderComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	ColliderComponent() = default;
	~ColliderComponent() = default;

	void TriggerOnCollisionEnter(ColliderComponent* collider);

	void TriggerOnCollisionStay(ColliderComponent* collider);

	void TriggerOnCollisionExit(ColliderComponent* collider);

	void SetOnCollisionEnter(std::function<void(ColliderComponent*)> onCollisionEnter) { onEnter_ = onCollisionEnter; }
	void SetOnCollisionStay(std::function<void(ColliderComponent*)> onCollisionEnter) { onStay_ = onCollisionEnter; }
	void SetOnCollisionExit(std::function<void(ColliderComponent*)> onCollisionEnter) { onExit_ = onCollisionEnter; }

	void UpdateSphere(const CollisionShape::Sphere& sphere);
	void UpdateOBB(const CollisionShape::OBB& obb);

	//--------- accessor -----------------------------------------------------

	void SetShape(const CollisionShape::Shapes& shape) { shape_ = shape; }

	void SetType(ColliderType type, ColliderType target);

	ColliderType GetType() const { return type_; }
	ColliderType GetTargetType() const { return targetType_; }

	const CollisionShape::Shapes& GetShape() const { return shape_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- using --------------------------------------------------------

	using CollisionCallback = std::function<void(ColliderComponent*)>;

	//--------- variables ----------------------------------------------------

	// コールバック関数
	CollisionCallback onEnter_ = nullptr;
	CollisionCallback onStay_ = nullptr;
	CollisionCallback onExit_ = nullptr;

	ColliderType type_;       // 自身の衝突タイプ
	ColliderType targetType_; // 衝突相手のタイプ

	CollisionShape::Shapes shape_; // 衝突判定を行う形状
};