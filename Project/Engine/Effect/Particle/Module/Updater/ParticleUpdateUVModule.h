#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	ParticleUpdateUVModule class
//============================================================================
class ParticleUpdateUVModule :
	public ICPUParticleUpdateModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleUpdateUVModule() = default;
	~ParticleUpdateUVModule() = default;

	void Init() override;

	void Execute(CPUParticle::ParticleData& particle, float deltaTime) override;

	void ImGui() override;

	// json
	Json ToJson() override;
	void FromJson(const Json& data) override;

	//--------- accessor -----------------------------------------------------

	const char* GetName() const override { return "UV"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleUpdateModuleID ID = ParticleUpdateModuleID::UV;
	ParticleUpdateModuleID GetID() const override { return ID; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 更新の種類
	enum class UpdateType {

		Lerp,
		Scroll
	};

	//--------- variables ----------------------------------------------------

	// UV座標
	ParticleCommon::LerpValue<Vector3> translation_;

	// スクロール加算値
	Vector2 scrollValue_;

	EasingType easing_;
	UpdateType updateType_;
};