#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionBody.h>

// c++
#include <string>
#include <optional>
// front
class Transform3DComponent;

//============================================================================
//	Collider class
//============================================================================
class Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Collider() = default;
	virtual ~Collider();

	void UpdateAllBodies(const Transform3DComponent& transform);

	// collision
	CollisionBody* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(CollisionBody* collisionBody);

	virtual void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) {};

	// json
	void BuildBodies(const Json& data);
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<CollisionBody*> bodies_;
	std::vector<CollisionShape::Shapes> bodyOffsets_;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// update
	void UpdateSphereBody(CollisionBody* body, const Transform3DComponent& transform, const CollisionShape::Sphere& offset);
	void UpdateAABBBody(CollisionBody* body, const Transform3DComponent& transform, const CollisionShape::AABB& offset);
	void UpdateOBBBody(CollisionBody* body, const Transform3DComponent& transform, const CollisionShape::OBB& offset);

	bool SetShapeParamFromJson(const std::string& shapeName, const Json& data);
	void SetTypeFromJson(CollisionBody& body, const Json& data);

	ColliderType ToColliderType(const std::string& name) const;
};