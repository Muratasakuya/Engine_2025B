#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ColliderComponent.h>
#include <Engine/Core/Lib/Collision.h>

// c++
#include <list>
#include <utility>
#include <unordered_set>

//============================================================================
//	CollisionManager class
//============================================================================
class CollisionManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	CollisionManager() = default;
	~CollisionManager() = default;

	ColliderComponent* AddComponent(const CollisionShape::Shapes& shape);

	void RemoveComponent(ColliderComponent* collider);

	void ClearAllCollision();

	void Update();

	//--------- accessor -----------------------------------------------------

	// singleton
	static CollisionManager* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& pair) const {
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};

	//--------- variables ----------------------------------------------------

	static CollisionManager* instance_;

	std::list<ColliderComponent*> colliders_;
	std::unordered_set<std::pair<ColliderComponent*, ColliderComponent*>, pair_hash> preCollisions_;

	//--------- functions ----------------------------------------------------

	bool IsColliding(ColliderComponent* colliderA, ColliderComponent* colliderB);

	void DrawCollider();
};