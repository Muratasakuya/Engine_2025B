#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/EntityComponentStructures.h>

// c++
#include <string>
#include <optional>
#include <typeinfo>

//============================================================================
//	BaseGameObject class
//============================================================================
class BaseGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseGameObject() = default;
	~BaseGameObject() = default;

	void CreateModel(const std::string& modelName,
		const std::optional<std::string>& animationName = std::nullopt);

	void Update();

	//--------- accessor -----------------------------------------------------

	// Transform
	void SetScale(const Vector3& scale) { object_->transform.scale = scale; }
	void SetRotate(const Quaternion& rotate) { object_->transform.rotation = rotate; }
	void SetTranslate(const Vector3& translate) { object_->transform.translation = translate; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	EntityData* object_;

	//--------- functions ----------------------------------------------------

	std::string GetObjectName() const;
};