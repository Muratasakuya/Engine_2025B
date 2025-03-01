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
//	Transform3D class
//============================================================================
class Transform3D {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Transform3D() = default;
	~Transform3D() = default;

	void Init();

	void UpdateMatrix();

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

	const Transform3D* parent = nullptr;
};