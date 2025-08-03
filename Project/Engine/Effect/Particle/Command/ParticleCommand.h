#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/ParticleModuleID.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <string>
#include <optional>
#include <variant>

//============================================================================
//	ParticleCommand
//============================================================================

// コマンド処理先
enum class ParticleCommandTarget {

	All,
	Spawner,
	Updater
};

// コマンドの識別ID
enum class ParticleCommandID {

	SetTranslation,   // 座標の設定
	SetQRotation,     // Quaternion回転の設定
	SetEulerRotation, // オイラー角回転の設定
	SetParent,        // 親の設定
};

// 特定のIDの指定
struct ParticleCommandFilter {

	std::optional<ParticleUpdateModuleID> updaterId;
};

// 使用できる型
using ParticleCommandValue = std::variant<
	bool, int32_t, uint32_t, float, Vector3,
	Vector4, Quaternion, Color, Matrix4x4>;

struct ParticleCommand {

	ParticleCommandTarget target;
	ParticleCommandID id;

	ParticleCommandFilter filter;
	ParticleCommandValue  value;
};