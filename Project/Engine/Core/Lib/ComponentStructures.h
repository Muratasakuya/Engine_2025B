#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxStructures.h>
#include <Engine/Core/CBuffer/Transform.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Component/ModelComponent.h>

// c++
#include <functional>

//============================================================================
//	ComponentStructures
//============================================================================

// entityID
using EntityID = uint32_t;

struct Object3D {

	EntityID id;
	Transform3DComponent* transform;
	std::vector<Material*> materials;
	ModelComponent* model;

	std::function<void()>* imguiFunc;
};