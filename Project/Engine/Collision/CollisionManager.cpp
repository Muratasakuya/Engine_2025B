#include "CollisionManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>

//============================================================================
//	CollisionManager classMethods
//============================================================================

CollisionManager* CollisionManager::instance_ = nullptr;

CollisionManager* CollisionManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new CollisionManager();
	}
	return instance_;
}

void CollisionManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

CollisionBody* CollisionManager::AddCollisionBody(const CollisionShape::Shapes& shape) {

	CollisionBody* collider = new CollisionBody();
	collider->SetShape(shape);
	colliders_.emplace_back(collider);

	return collider;
}

void CollisionManager::RemoveCollisionBody(CollisionBody* collider) {

	auto itA = std::find(colliders_.begin(), colliders_.end(), collider);
	if (itA != colliders_.end()) {
		colliders_.erase(itA);
	}

	for (auto itB = preCollisions_.begin(); itB != preCollisions_.end();) {
		if (itB->first == collider || itB->second == collider) {

			itB = preCollisions_.erase(itB);
		} else {
			++itB;
		}
	}
}

void CollisionManager::ClearAllCollision() {

	colliders_.clear();
	preCollisions_.clear();
}

void CollisionManager::Update() {

	if (colliders_.empty()) {
		return;
	}

	std::unordered_set<std::pair<CollisionBody*, CollisionBody*>, pair_hash> currentCollisions;

	for (auto itA = colliders_.begin(); itA != colliders_.end(); ++itA) {

		auto itB = itA;
		++itB;

		for (; itB != colliders_.end(); ++itB) {

			CollisionBody* colliderA = *itA;
			CollisionBody* colliderB = *itB;

			if (colliderA->GetTargetType() != colliderB->GetType() &&
				colliderB->GetTargetType() != colliderA->GetType()) {
				continue;
			}

			if (IsColliding(colliderA, colliderB)) {
				currentCollisions.insert({ colliderA, colliderB });

				if (preCollisions_.find({ colliderA, colliderB }) == preCollisions_.end()) {

					colliderA->TriggerOnCollisionEnter(colliderB);
					colliderB->TriggerOnCollisionEnter(colliderA);
				} else {

					colliderA->TriggerOnCollisionStay(colliderB);
					colliderB->TriggerOnCollisionStay(colliderA);
				}
			}
		}
	}

	for (auto& collision : preCollisions_) {
		if (currentCollisions.find(collision) == currentCollisions.end()) {

			collision.first->TriggerOnCollisionExit(collision.second);
			collision.second->TriggerOnCollisionExit(collision.first);
		}
	}

	preCollisions_ = currentCollisions;

	DrawCollider();
}

bool CollisionManager::IsColliding(CollisionBody* colliderA, CollisionBody* colliderB) {

	// 形状取得
	const auto& shapeA = colliderA->GetShape();
	const auto& shapeB = colliderB->GetShape();

	return std::visit([&](const auto& shapeA, const auto& shapeB) {

		using ShapeTypeA = std::decay_t<decltype(shapeA)>;
		using ShapeTypeB = std::decay_t<decltype(shapeB)>;

		if constexpr (std::is_same_v<ShapeTypeA, CollisionShape::Sphere> && std::is_same_v<ShapeTypeB, CollisionShape::Sphere>) {

			return Collision::SphereToSphere(shapeA, shapeB);
		} else if constexpr (std::is_same_v<ShapeTypeA, CollisionShape::Sphere> && std::is_same_v<ShapeTypeB, CollisionShape::OBB>) {

			return Collision::SphereToOBB(shapeA, shapeB);
		} else if constexpr (std::is_same_v<ShapeTypeA, CollisionShape::OBB> && std::is_same_v<ShapeTypeB, CollisionShape::Sphere>) {

			return Collision::SphereToOBB(shapeB, shapeA);
		} else if constexpr (std::is_same_v<ShapeTypeA, CollisionShape::OBB> && std::is_same_v<ShapeTypeB, CollisionShape::OBB>) {

			return Collision::OBBToOBB(shapeA, shapeB);
		} else {

			return false;
		}
		}, shapeA, shapeB);
}

void CollisionManager::DrawCollider() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	if (!lineRenderer) return;

	for (auto& collider : colliders_) {

		const auto& shape = collider->GetShape();
		Vector3 center = Vector3::AnyInit(0.0f);
		if (std::holds_alternative<CollisionShape::Sphere>(shape)) {

			center = std::get<CollisionShape::Sphere>(shape).center;
		}else if(std::holds_alternative<CollisionShape::OBB>(shape)) {

			center = std::get<CollisionShape::OBB>(shape).center;
		}

		// コライダーが衝突しているかどうかを判定
		bool isColliding = false;
		for (const auto& collisionPair : preCollisions_) {
			if (collisionPair.first == collider || collisionPair.second == collider) {
				isColliding = true;
				break;
			}
		}

		// 衝突状態に応じた色を設定
		Color color = isColliding ? Color::Red() : Color::Green();

		std::visit([&](const auto& shapeData) {
			using ShapeType = std::decay_t<decltype(shapeData)>;

			if constexpr (std::is_same_v<ShapeType, CollisionShape::Sphere>) {

				lineRenderer->DrawSphere(8, shapeData.radius, center, color);
			} else if constexpr (std::is_same_v<ShapeType, CollisionShape::OBB>) {

				lineRenderer->DrawOBB(shapeData, color);
			}
			}, shape);
	}
}