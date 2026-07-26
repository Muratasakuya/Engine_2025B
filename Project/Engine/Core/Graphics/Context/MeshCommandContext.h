#pragma once

//============================================================================
//	include
//============================================================================

// directX
#include <d3d12.h>
// c++
#include <cstdint>
namespace SakuEngine {

// front

class DxCommand;

/// <summary>
/// Mesh Shader用の頂点/メッシュレット/プリミティブバッファを設定し、DispatchMeshを発行する描画コンテキスト。
/// </summary>
class MeshCommandContext {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshCommandContext() = default;
	~MeshCommandContext() = default;

	/// <summary>
	/// 指定メッシュのバッファをRoot SRV/CBVへ設定し、インスタンス数に応じてMesh Shaderをディスパッチする。
	/// </summary>
	/// <param name="commandList">描画コマンドを記録するDirect3D12コマンドリスト。</param>
	/// <param name="instanceCount">描画するインスタンス数。0の場合は何も発行しない。</param>
	/// <param name="meshIndex">メッシュ内のサブメッシュインデックス。</param>
	/// <param name="mesh">描画対象のメッシュデータ。</param>
	void DispatchMesh(ID3D12GraphicsCommandList6* commandList,
		UINT instanceCount, uint32_t meshIndex, class IMesh* mesh);
};

}; // SakuEngine
