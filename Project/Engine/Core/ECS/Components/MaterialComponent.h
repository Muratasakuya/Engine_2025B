#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Utility/SimpleAnimation.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	UVTransform
//============================================================================

struct UVTransform {

	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	// operator
	bool operator==(const UVTransform& other) const {

		return scale == other.scale &&
			rotate == other.rotate &&
			translate == other.translate;
	}
};

//============================================================================
//	MaterialComponent class
//============================================================================
class MaterialComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialComponent() = default;
	~MaterialComponent() = default;

	void Init();

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- variables ----------------------------------------------------

	Material material;

	UVTransform uvTransform;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	UVTransform prevUVTransform;
};

//============================================================================
//	SpriteMaterialComponent class
//============================================================================
class SpriteMaterialComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteMaterialComponent() = default;
	~SpriteMaterialComponent() = default;

	void Init(ID3D12Device* device);

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- variables ----------------------------------------------------

	SpriteMaterial material;

	UVTransform uvTransform;

	//--------- accessor -----------------------------------------------------

	const DxConstBuffer<SpriteMaterial>& GetBuffer() const { return buffer_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	UVTransform prevUVTransform;

	// buffer
	DxConstBuffer<SpriteMaterial> buffer_;
};