#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/Collision.h>

// c++
#include <cstdint>
#include <optional>
#include <functional>
// front
class BaseGameObject;
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

	//--------- using --------------------------------------------------------

	using CollisionCallback = std::function<void(ColliderComponent*)>;

	//--------- functions ----------------------------------------------------

	ColliderComponent() = default;
	~ColliderComponent() = default;

	void TriggerOnCollisionEnter(ColliderComponent* collider);

	void TriggerOnCollisionStay(ColliderComponent* collider);

	void TriggerOnCollisionExit(ColliderComponent* collider);

	void Update();

	void SetOnCollisionEnter(std::function<void(ColliderComponent*)> onCollisionEnter) { onEnter_ = onCollisionEnter; }
	void SetOnCollisionStay(std::function<void(ColliderComponent*)> onCollisionEnter) { onStay_ = onCollisionEnter; }
	void SetOnCollisionExit(std::function<void(ColliderComponent*)> onCollisionEnter) { onExit_ = onCollisionEnter; }

	//--------- variables ----------------------------------------------------

	// コールバック関数
	CollisionCallback onEnter_ = nullptr;
	CollisionCallback onStay_ = nullptr;
	CollisionCallback onExit_ = nullptr;

	ColliderID id_; //* 自身のid

	Vector3 centerPos_; //* 衝突判定用中心座標

	float radius_; //* 円衝突用の大きさ

	Quaternion rotate_; //* OBB衝突用の回転
	Vector3 size_;      //* OBB衝突用の大きさ

	ColliderType type_;       // 自身の衝突タイプ
	ColliderType targetType_; // 衝突相手のタイプ

	std::optional<CollisionShape::Shapes> shape_ = std::nullopt; // 衝突判定を行う形状
};