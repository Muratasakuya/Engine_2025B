#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>

// directX
#include <d3d12.h>

//============================================================================
//	RayTracingStructures
//============================================================================

struct RayTracingInstance {

	Matrix4x4 matrix;
	UINT instanceID : 24;  // SV_InstanceID
	UINT hitGroupIdx : 24; // ShaderTableのHitGroupインデックス
	UINT mask : 8;         // レイマスク
	UINT flags : 8;        // D3D12_RAYTRACING_INSTANCE_FLAG_*
	ID3D12Resource* blas;  // 対応するBLAS
	uint32_t meshIndex;
};

struct RaySceneForGPU {

	Vector3 cameraPos;            // カメラ座標
	float pad1;
	Matrix4x4 cameraRotateMatrix; // カメラのワールド行列
	Vector3 lightDirection;       // 光源の向き
	float pad2;
};