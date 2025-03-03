#include "TemplateObject3D.h"

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	BaseGameObject::CreateModel("teapot");
	collider_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
}