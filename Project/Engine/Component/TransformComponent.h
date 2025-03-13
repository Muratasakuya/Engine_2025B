#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/CBufferStructures.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Quaternion.h>

//============================================================================
//	UVTransform
//============================================================================

struct UVTransform {

	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

//============================================================================
//	Transform3DComponent class
//============================================================================
class Transform3DComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3DComponent() = default;
	~Transform3DComponent() = default;

	void Init();

	void UpdateMatrix();

	void ImGui(float itemSize);

	//--------- accessor -----------------------------------------------------

	Vector3 GetWorldPos() const;

	Vector3 GetForward() const;

	Vector3 GetBack() const;

	Vector3 GetRight() const;

	Vector3 GetLeft() const;

	Vector3 GetUp() const;

	Vector3 GetDown() const;

	//--------- variables ----------------------------------------------------

	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;

	TransformationMatrix matrix;

	const Transform3DComponent* parent = nullptr;
};