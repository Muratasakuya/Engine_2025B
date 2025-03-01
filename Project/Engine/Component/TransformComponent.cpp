#include "TransformComponent.h"

//============================================================================
//	TransformComponent classMethods
//============================================================================

void Transform3D::Init() {

	scale = Vector3::AnyInit(1.0f);
	rotation.Init();
	translation.Init();
}

void Transform3D::UpdateMatrix() {

	matrix.Update(parent, scale, rotation, translation);
}

Vector3 Transform3D::GetWorldPos() const {

	Vector3 worldPos{};
	worldPos.x = matrix.world.m[3][0];
	worldPos.y = matrix.world.m[3][1];
	worldPos.z = matrix.world.m[3][2];

	return worldPos;
}

Vector3 Transform3D::GetForward() const {
	return Vector3(matrix.world.m[2][0], matrix.world.m[2][1], matrix.world.m[2][2]).Normalize();
}

Vector3 Transform3D::GetBack() const {
	return Vector3(-GetForward().x, -GetForward().y, -GetForward().z);
}

Vector3 Transform3D::GetRight() const {
	return Vector3(matrix.world.m[0][0], matrix.world.m[0][1], matrix.world.m[0][2]).Normalize();
}

Vector3 Transform3D::GetLeft() const {
	return Vector3(-GetRight().x, -GetRight().y, -GetRight().z);
}

Vector3 Transform3D::GetUp() const {
	return Vector3(matrix.world.m[1][0], matrix.world.m[1][1], matrix.world.m[1][2]).Normalize();
}

Vector3 Transform3D::GetDown() const {
	return Vector3(-GetUp().x, -GetUp().y, -GetUp().z);
}
