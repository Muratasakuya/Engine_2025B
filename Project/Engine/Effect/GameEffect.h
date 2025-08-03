#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Data/Transform.h>

// c++
#include <string>
// front
class ParticleSystem;

//============================================================================
//	GameEffect class
//============================================================================
class GameEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameEffect() = default;
	~GameEffect() = default;

	// ParticleSystemの作成
	void CreateParticleSystem(const std::string& filePath);

	// リセット
	void ResetEmitFlag();

	//----------- emit -------------------------------------------------------

	// 一定間隔
	void FrequencyEmit();
	// 強制発生
	void Emit(bool emitOnce = false);

	//---------- setter ------------------------------------------------------

	// transform
	void SetTransform(const Matrix4x4& matrix);
	void SetParent(const BaseTransform& transform);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ParticleSystem* particleSystem_;

	// 1回だけ発生させるか
	bool emitOnce_;

	// 親を設定済みか
	bool hasParent_;
};