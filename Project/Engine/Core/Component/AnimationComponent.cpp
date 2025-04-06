#include "AnimationComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	AnimationComponent classMethods
//============================================================================

void AnimationComponent::Init(const std::string& animationName, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	animationData_[animationName] = asset_->GetAnimationData(animationName);
	skeleton_[animationName] = asset_->GetSkeletonData(animationName);
	skeleton_[animationName].value().name = animationName;
	skinCluster_[animationName] = asset_->GetSkinClusterData(animationName);

	transitionDuration_ = 0.4f;
}

void AnimationComponent::Update() {

	animationFinish_ = false;

	//========================================================================
	// 通常のAnimation再生
	//========================================================================
	if (!inTransition_) {
		if (roopAnimation_) {

			currentAnimationTimer_ += GameTimer::GetScaledDeltaTime();
			currentAnimationTimer_ = std::fmod(currentAnimationTimer_, animationData_[currentAnimationName_].duration);

			animationProgress_ = currentAnimationTimer_ / animationData_[currentAnimationName_].duration;
		} else {
			if (animationData_[currentAnimationName_].duration > currentAnimationTimer_) {
				currentAnimationTimer_ += GameTimer::GetScaledDeltaTime();
			}
			if (currentAnimationTimer_ >= animationData_[currentAnimationName_].duration) {
				currentAnimationTimer_ = animationData_[currentAnimationName_].duration;

				animationFinish_ = true;
			}

			animationProgress_ = currentAnimationTimer_ / animationData_[currentAnimationName_].duration;
		}

		if (skeleton_[currentAnimationName_]) {

			asset_->ApplyAnimation(skeleton_[currentAnimationName_].value().name, currentAnimationTimer_);
			asset_->SkeletonUpdate(skeleton_[currentAnimationName_].value().name);
			asset_->SkinClusterUpdate(skeleton_[currentAnimationName_].value().name);
		}
	}
	//========================================================================
	// 遷移中のAnimation再生
	//========================================================================
	else {

		// 遷移時間を進める
		transitionTimer_ += GameTimer::GetScaledDeltaTime();
		float alpha = transitionTimer_ / transitionDuration_;
		if (alpha > 1.0f) {

			alpha = 1.0f;
		}

		// AnimationをBlendして更新する
		asset_->BlendAnimation(skeleton_[oldAnimationName_].value().name, oldAnimationTimer_,
			skeleton_[nextAnimationName_].value().name, nextAnimationTimer_, alpha);
		asset_->SkeletonUpdate(skeleton_[oldAnimationName_].value().name);
		asset_->SkinClusterUpdate(skeleton_[oldAnimationName_].value().name);

		// 遷移終了
		if (alpha >= 1.0f) {

			inTransition_ = false;
			currentAnimationName_ = nextAnimationName_;
			currentAnimationTimer_ = nextAnimationTimer_;
		}
	}
}

void AnimationComponent::SetPlayAnimation(const std::string& animationName, bool roopAnimation) {

	// Animationの再生設定
	currentAnimationTimer_ = 0.0f;
	currentAnimationName_ = animationName;
	roopAnimation_ = roopAnimation;
}