#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionBody.h>
#include <Engine/Collision/CollisionGeometry.h>

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

	CollisionBody* AddCollisionBody(const CollisionShape::Shapes& shape);

	void RemoveCollisionBody(CollisionBody* collider);

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

	std::list<CollisionBody*> colliders_;
	std::unordered_set<std::pair<CollisionBody*, CollisionBody*>, pair_hash> preCollisions_;

	//--------- functions ----------------------------------------------------

	bool IsColliding(CollisionBody* colliderA, CollisionBody* colliderB);

	void DrawCollider();

	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;
};