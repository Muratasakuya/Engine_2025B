#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>

// imgui
#include <imgui.h>

//============================================================================
//	enum class
//============================================================================

// 発生処理
enum class ParticleSpawnModuleID {

	Sphere,
	Hemisphere,
	Box,
	Cone,
	Count,
};

// 更新処理
enum class ParticleUpdateModuleID {

	Color,
	Velocity,
	Rotation,
	Scale,
	Gravity,
	Parent,
	Count,
};

//============================================================================
//	ICPUParticleModule class
//============================================================================
class ICPUParticleModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ICPUParticleModule() = default;
	virtual ~ICPUParticleModule() = default;

	virtual void Init() = 0;

	virtual void ImGui() = 0;

	//--------- accessor -----------------------------------------------------

	virtual const char* GetName() const = 0;
};