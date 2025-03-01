#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>

//============================================================================
//	IOVertexBuffer class
//============================================================================
class IOVertexBuffer :
	public DxConstBuffer<ModelVertexData> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IOVertexBuffer() = default;
	~IOVertexBuffer() = default;

	void Init(UINT vertexNum, ID3D12Resource* vertexResource,
		ID3D12Device* device, class SRVManager* SRVManager);

	//--------- accessor -----------------------------------------------------

	const InputModelVertex& GetInputData() const { return inputVertex_; }
	const OutputModelVertex& GetOutputData() const { return outputVertex_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	InputModelVertex inputVertex_;
	OutputModelVertex outputVertex_;
};