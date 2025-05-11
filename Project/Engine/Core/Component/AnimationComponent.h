#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>

// front
class Asset;

//============================================================================
//	AnimationComponent class
//============================================================================
class AnimationComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationComponent() = default;
	~AnimationComponent() = default;

	void Init(const std::string& animationName, Asset* asset);

	void Update();

	//--------- accessor -----------------------------------------------------

	void SetAnimationData(const std::string& animationName);

	// animation再生
	void SetPlayAnimation(const std::string& animationName, bool roopAnimation);

	bool IsTransition() const { return inTransition_; }

	const std::vector<WellForGPU>& GetWellForGPU() const { return skinCluster_.mappedPalette; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// animationDataはstringで名前ごとに保存して使うようにする
	std::unordered_map<std::string, AnimationData> animationData_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;

	std::string currentAnimationName_; // 現在のAnimationの名前
	float currentAnimationTimer_;      // 現在のAnimation経過時間
	bool roopAnimation_;               // ループ再生するかどうか

	bool animationFinish_; // 現在のAnimationが終了したかどうか

	bool inTransition_;        // 遷移中かどうか
	float transitionTimer_;    // 遷移管理タイマー
	float transitionDuration_; // 遷移時間

	// 切り替え前のAnimation
	std::string oldAnimationName_;
	float oldAnimationTimer_;
	// 切り替え後のAnimation
	std::string nextAnimationName_;
	float nextAnimationTimer_;

	// Animationの経過率
	float animationProgress_;
};