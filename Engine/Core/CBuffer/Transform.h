#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>
#include <Engine/Core/Lib/CBufferStructures.h>

// front
class Asset;

//============================================================================
//	AnimationTransform class
//============================================================================
class AnimationTransform :
	public DxConstBuffer<TransformationMatrix> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationTransform() = default;
	~AnimationTransform() = default;

	void Init(const std::string& animationName, Asset* asset);

	void Update(const TransformationMatrix& matrix);

	//--------- accessor -----------------------------------------------------

	bool IsTransition() const { return inTransition_; }

	bool AnimationFinish() const { return animationFinish_; }
	std::string GetCurrentAnimationName() const { return currentAnimationName_; }

	float GetAnimationProgress() const { return animationProgress_; }

	// 最初のAnimation再生
	void SetPlayAnimation(const std::string& animationName, bool roopAnimation);

	// 切り替えAnimation
	void SwitchAnimation(const std::string& nextAnimName, bool loopAnimation, float transitionDuration);

	// Animationの設定
	void SetAnimationData(const std::string& animationName);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	std::unordered_map<std::string, AnimationData> animationData_;
	std::unordered_map<std::string, std::optional<Skeleton>> skeleton_;
	std::unordered_map<std::string, SkinCluster> skinCluster_;

	std::string currentAnimationName_; //* 現在のAnimationの名前
	float currentAnimationTimer_;      //* 現在のAnimation経過時間
	bool roopAnimation_;               //* ループ再生するかどうか

	bool animationFinish_; //* 現在のAnimationが終了したかどうか

	//* Transition *//

	bool inTransition_;        //* 遷移中かどうか
	float transitionTimer_;    //* 遷移管理タイマー
	float transitionDuration_; //* 遷移時間

	// 切り替え前のAnimation
	std::string oldAnimationName_;
	float oldAnimationTimer_;
	// 切り替え後のAnimation
	std::string nextAnimationName_;
	float nextAnimationTimer_;

	// Animationの経過率
	float animationProgress_;
};