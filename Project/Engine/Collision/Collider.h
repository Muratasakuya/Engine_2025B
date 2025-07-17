#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionBody.h>

// c++
#include <string>
#include <optional>
// front
class Transform3D;

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

	void UpdateAllBodies(const Transform3D& transform);

	// collision
	CollisionBody* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(CollisionBody* collisionBody);

	virtual void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) {};

	// imgui
	void ImGui(float itemWidth);

	// json
	// material
	void ApplyBodyOffset(const Json& data);
	void SaveBodyOffset(Json& data);

	// json
	void BuildBodies(const Json& data);

	//--------- accessor -----------------------------------------------------

	void SetIsChild(bool isChild) { isChild_ = isChild; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<CollisionBody*> bodies_;
	std::vector<CollisionShape::Shapes> bodyOffsets_;

	bool isChild_; // 子の場合は行列を使用して更新する

	//--------- functions ----------------------------------------------------

	int ToIndexType(ColliderType type);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// update
	void UpdateSphereBody(CollisionBody* body, const Transform3D& transform, const CollisionShape::Sphere& offset);
	void UpdateAABBBody(CollisionBody* body, const Transform3D& transform, const CollisionShape::AABB& offset);
	void UpdateOBBBody(CollisionBody* body, const Transform3D& transform, const CollisionShape::OBB& offset);

	bool SetShapeParamFromJson(const std::string& shapeName, const Json& data);
	void SetTypeFromJson(CollisionBody& body, const Json& data);

	ColliderType ToColliderType(const std::string& name) const;

	void EditSphereBody(uint32_t index, CollisionShape::Sphere& offset);
	void EditAABBBody(uint32_t index, CollisionShape::AABB& offset);
	void EditOBBBody(uint32_t index, CollisionShape::OBB& offset);
};