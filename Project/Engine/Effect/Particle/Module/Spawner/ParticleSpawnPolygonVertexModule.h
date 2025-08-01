#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleSpawnModule.h>

//============================================================================
//	ParticleSpawnPolygonVertexModule class
//============================================================================
class ParticleSpawnPolygonVertexModule :
	public ICPUParticleSpawnModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleSpawnPolygonVertexModule() = default;
	~ParticleSpawnPolygonVertexModule() = default;

	void Init() override;

	void Execute(std::list<CPUParticle::ParticleData>& particles) override;

	void UpdateEmitter() override;
	void DrawEmitter() override;

	void ImGui() override;

	// json
	Json ToJson() override;
	void FromJson(const Json& data) override;

	//--------- accessor -----------------------------------------------------

	void SetTransform(const Matrix4x4& matrix) override;

	const char* GetName() const override { return "SpawnPolygonVertex"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleSpawnModuleID ID = ParticleSpawnModuleID::PolygonVertex;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// パーティクル間の補間処理を行うか
	bool isInterpolate_;
	// 移動していなければ発生させないか
	bool notMoveEmit_;

	Vector3 emitterRotation_;
	Vector3 translation_;

	// 多角形
	int vertexCount_; // 頂点数
	float scale_;     // サイズ

	ParticleValue<uint32_t> emitPerVertex_;   // 各頂点の発生数
	std::vector<Vector3> prevVertices_;       // 前フレームの頂点位置
	ParticleValue<float> interpolateSpacing_; // パーティクル間の距離

	//--------- functions ----------------------------------------------------

	// emit
	bool EnableEmit();
	void InterpolateEmit(std::list<CPUParticle::ParticleData>& particles);
	void NoneEmit(std::list<CPUParticle::ParticleData>& particles);

	// helper
	std::vector<Vector3> CalcVertices() const;
};