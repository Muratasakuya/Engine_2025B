#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>

// c++
#include <execution>
#include <ranges> 

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

	void Update(const Matrix4x4& worldMatrix);

	void ImGui(float itemSize);

	//--------- accessor -----------------------------------------------------

	// 新しいanimationの設定
	void SetAnimationData(const std::string& animationName);
	// animationのkeyframeのイベント位置
	void SetKeyframeEvent(const std::string& fileName);

	// animation再生
	void SetPlayAnimation(const std::string& animationName, bool roopAnimation);
	// animationリセット
	void ResetAnimation();
	// 切り替えAnimation
	void SwitchAnimation(const std::string& nextAnimName, bool loopAnimation, float transitionDuration);

	// 親として更新するjointを設定
	void SetParentJoint(const std::string& jointName);

	bool IsTransition() const { return inTransition_; }
	bool IsAnimationFinished() const { return animationFinish_; }
	bool IsHitEffectKey(uint32_t frameIndex) const;
	int GetRepeatCount() const { return repeatCount_; }
	float GetAnimationDuration(const std::string& animationName) const;

	const std::vector<WellForGPU>& GetWellForGPU() const { return skinCluster_.mappedPalette; }
	const Skeleton& GetSkeleton() const { return skeleton_; }

	const Transform3DComponent* FindJointTransform(const std::string& name) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// animationDataはstringで名前ごとに保存して使うようにする
	std::unordered_map<std::string, AnimationData> animationData_;
	// 使用するjointをstd::stringで名前ごとに記録
	std::unordered_map<std::string, std::vector<const NodeAnimation*>> jointAnimationTracks_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;

	// keyframeイベント
	std::unordered_map<std::string, std::vector<int>> eventKeyTables_;

	std::string currentAnimationName_; // 現在のAnimationの名前
	float currentAnimationTimer_;      // 現在のAnimation経過時間
	bool roopAnimation_;               // ループ再生するかどうか

	bool animationFinish_; // 現在のanimationが終了したかどうか
	int repeatCount_;      // リピート回数

	bool inTransition_;        // 遷移中かどうか
	float transitionTimer_;    // 遷移管理タイマー
	float transitionDuration_; // 遷移時間

	// 切り替え前のanimation
	std::string oldAnimationName_;
	float oldAnimationTimer_;
	// 切り替え後のanimation
	std::string nextAnimationName_;
	float nextAnimationTimer_;

	// animationの経過率
	float animationProgress_;

	//--------- variables ----------------------------------------------------

	// joint更新
	void ApplyAnimation();
	void UpdateSkeleton(const Matrix4x4& worldMatrix);
	void UpdateSkinCluster();

	// blend処理
	void BlendAnimation(Skeleton& skeleton,
		const AnimationData& oldAnimationData, float oldAnimTime,
		const AnimationData& nextAnimationData, float nextAnimTime, float alpha);

	// helper
	int CurrentFrameIndex() const;
	void DrawEventTimeline(const std::vector<int>& frames, int currentFrame,
		int totalFrames, float barWidth, float barHeight);
};