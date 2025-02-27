#include "Transform.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	AnimationTransform classMethods
//============================================================================

void AnimationTransform::Init(const std::string& animationName, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	currentAnimationName_ = animationName;

	transitionDuration_ = 0.4f;
	animationFinish_ = false;
}

void AnimationTransform::Update(const TransformationMatrix& matrix) {

	animationFinish_ = false;

	//============================================================================
	//	通常のAnimation再生
	//============================================================================
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
	//============================================================================
	//	遷移中のAnimation再生
	//============================================================================
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

	DxConstBuffer::TransferData(matrix);
}

void AnimationTransform::SetPlayAnimation(const std::string& animationName, bool roopAnimation) {

	// Animationの再生設定
	currentAnimationTimer_ = 0.0f;
	currentAnimationName_ = animationName;
	roopAnimation_ = roopAnimation;
}

void AnimationTransform::SwitchAnimation(const std::string& nextAnimName, bool loopAnimation, float transitionDuration) {

	// 現在のAnimationを設定
	oldAnimationName_ = currentAnimationName_;
	oldAnimationTimer_ = currentAnimationTimer_;

	// 次のAnimationを設定
	nextAnimationName_ = nextAnimName;
	nextAnimationTimer_ = 0.0f;

	// 遷移開始
	inTransition_ = true;
	transitionTimer_ = 0.0f;
	transitionDuration_ = transitionDuration;

	roopAnimation_ = loopAnimation;
}

void AnimationTransform::SetAnimationData(const std::string& animationName) {

	animationData_[animationName] = asset_->GetAnimationData(animationName);
	skeleton_[animationName] = asset_->GetSkeletonData(animationName);
	skeleton_[animationName].value().name = animationName;
	skinCluster_[animationName] = asset_->GetSkinClusterData(animationName);
}