#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/CBuffer/DxConstBuffer.h>

//============================================================================
//	PostProcessBuffer class
//============================================================================
template <typename T>
class PostProcessBuffer :
	public DxConstBuffer<T> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PostProcessBuffer() = default;
	~PostProcessBuffer() = default;

	void Init(ID3D12Device* device, UINT rootIndex);

	void Update();

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetProperties(const T& properties);

	UINT GetRootIndex() const { return rootIndex_; };
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	T properties_;

	UINT rootIndex_ = 0;
};

//============================================================================
//	PostProcessBuffer templateMethods
//============================================================================

template<typename T>
inline void PostProcessBuffer<T>::Init(ID3D12Device* device, UINT rootIndex) {

	rootIndex_ = rootIndex;

	DxConstBuffer<T>::CreateConstBuffer(device);
}

template<typename T>
inline void PostProcessBuffer<T>::Update() {

	DxConstBuffer<T>::TransferData(properties_);
}

template<typename T>
inline void PostProcessBuffer<T>::ImGui() {

	properties_.ImGui();
}

template<typename T>
inline void PostProcessBuffer<T>::SetProperties(const T& properties) {

	properties_ = properties;
}