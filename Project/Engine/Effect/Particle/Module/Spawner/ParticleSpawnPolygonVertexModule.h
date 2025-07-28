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

	//--------- accessor -----------------------------------------------------

	const char* GetName() const override { return "SpawnPolygonVertex"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleSpawnModuleID ID = ParticleSpawnModuleID::PolygonVertex;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 emitterRotation_;
	Vector3 translation_;

	// 多角形
	int vertexCount_; // 頂点数
	float scale_;     // サイズ

	ParticleValue<uint32_t> emitPerVertex_; // 各頂点の発生数
	std::vector<Vector3> prevVertices_;     // 前フレームの頂点位置

	//--------- functions ----------------------------------------------------

	std::vector<Vector3> CalcVertices() const;
};