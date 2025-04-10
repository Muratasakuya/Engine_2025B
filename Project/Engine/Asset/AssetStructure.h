#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector2.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Vector4.h>
#include <Lib/MathUtils/Matrix4x4.h>
#include <Lib/MathUtils/Quaternion.h>
#include <Engine/Core/Graphics/Lib/ComPtr.h>

// assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
// directX
#include <d3d12.h>
// c++
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <optional>
#include <utility>
#include <span>
#include <map>

//============================================================================
//	Asset structures
//============================================================================
struct ModelVertexData {

	Vector4 pos;
	Vector2 texcoord;
	Vector3 normal;
};

struct SpriteVertexData {

	Vector2 pos;
	Vector2 texcoord;
};

struct InputModelVertex {

	std::vector<ModelVertexData> data;
	uint32_t srvIndex;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle;
};
struct OutputModelVertex {

	std::vector<ModelVertexData> data;
	uint32_t uavIndex;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> uavHandle;
};

struct ModelTransform {

	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

struct MeshModelData {

	std::vector<ModelVertexData> vertices;
	std::vector<uint32_t> indices;
	std::optional<std::string> textureName;
};

struct Node {

	ModelTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct VertexWeightData {

	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {

	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

struct ModelData {

	std::vector<MeshModelData> meshes;
	std::map<std::string, JointWeightData> skinClusterData;
	Node rootNode;

	std::string fullPath;
};

template <typename tValue>
struct Keyframe {

	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template <typename tValue>
struct AnimationCurve {

	std::vector<Keyframe<tValue>> keyframes;
};
struct NodeAnimation {

	AnimationCurve<Vector3> scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> translate;
};

struct AnimationData {

	float duration;                                                // アニメーション全体の尺
	std::map<std::string, NodeAnimation> nodeAnimations; // NodeAnimationの集合、Node名で引けるようにしておく
};
struct Joint {

	ModelTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;   // skeletonSpaceでの変換行列
	std::string name;
	std::vector<int32_t> children;   // 子JointのIndexのリスト。いなければ空
	int32_t index;                   // 自身のIndex
	std::optional<int32_t> parent;   // 親JointのIndex。いなければnull
};
struct Skeleton {

	int32_t root;                            // RootJointのIndex
	std::map<std::string, int32_t> jointMap; // joint名とIndexの辞書
	std::vector<Joint> joints;               // 所属しているジョイント
	std::string name;
};

// サイダイ4Jointの影響を受ける
const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {

	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};
struct WellForGPU {

	Matrix4x4 skeletonSpaceMatrix;                 // 位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
};
struct SkinningInformation {

	uint32_t numVertices;
};
struct SkinCluster {

	std::vector<Matrix4x4> inverseBindPoseMatrices;

	ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle;

	ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};
struct AnimationBlendState {

	bool isBlending = false;    // ブレンド中か
	float blendTimer = 0.0f;    // 現在のブレンド経過時間
	float blendDuration = 0.0f; // ブレンドにかける総時間

	// 今まで再生していたアニメーション
	std::string fromAnimation;
	float fromAnimationTime = 0.0f;

	// 次に再生するアニメーション
	std::string toAnimation;
	float toAnimationTime = 0.0f;
};