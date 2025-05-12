#include "AnimationComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Game/Time/GameTimer.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationComponent classMethods
//============================================================================

void AnimationComponent::Init(const std::string& animationName, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// 骨の情報とクラスターを渡す
	skeleton_ = asset_->GetSkeletonData(animationName);
	skinCluster_ = asset_->GetSkinClusterData(animationName);

	// 初期値
	transitionDuration_ = 0.4f;
}

void AnimationComponent::Update() {

	// animationがなにも設定されていなければ何もしない
	if (animationData_.empty()) {
		return;
	}

	animationFinish_ = false;

	//========================================================================
	// 通常のAnimation再生
	//========================================================================
	if (!inTransition_) {
		// ループ再生かしないか
		if (roopAnimation_) {

			// 経過時間を進める
			currentAnimationTimer_ += GameTimer::GetScaledDeltaTime();
			currentAnimationTimer_ = std::fmod(currentAnimationTimer_, animationData_[currentAnimationName_].duration);

			// 進行度を計算
			animationProgress_ = currentAnimationTimer_ / animationData_[currentAnimationName_].duration;
		} else {
			// 経過時間が最大にいくまで時間を進める
			if (animationData_[currentAnimationName_].duration > currentAnimationTimer_) {

				currentAnimationTimer_ += GameTimer::GetScaledDeltaTime();
			}

			// 経過時間に達したら終了させる
			if (currentAnimationTimer_ >= animationData_[currentAnimationName_].duration) {
				currentAnimationTimer_ = animationData_[currentAnimationName_].duration;

				animationFinish_ = true;
			}

			animationProgress_ = currentAnimationTimer_ / animationData_[currentAnimationName_].duration;
		}

		// jointの値を更新する
		asset_->ApplyAnimation(skeleton_,
			animationData_[currentAnimationName_], currentAnimationTimer_);
		asset_->SkeletonUpdate(skeleton_);

		// bufferに渡す値を更新する
		asset_->SkinClusterUpdate(skinCluster_, skeleton_);
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

		// animationをblendしてjointの値を更新する
		asset_->BlendAnimation(skeleton_,
			animationData_[oldAnimationName_], oldAnimationTimer_,
			animationData_[nextAnimationName_], nextAnimationTimer_, alpha);
		asset_->SkeletonUpdate(skeleton_);

		// bufferに渡す値を更新する
		asset_->SkinClusterUpdate(skinCluster_, skeleton_);

		// 遷移終了
		if (alpha >= 1.0f) {

			inTransition_ = false;
			currentAnimationName_ = nextAnimationName_;
			currentAnimationTimer_ = nextAnimationTimer_;
		}
	}
}

void AnimationComponent::SetAnimationData(const std::string& animationName) {

	if (!Algorithm::Find(animationData_, animationName)) {

		animationData_[animationName] = asset_->GetAnimationData(animationName);
	}
}

void AnimationComponent::SetPlayAnimation(const std::string& animationName, bool roopAnimation) {

	// Animationの再生設定
	currentAnimationTimer_ = 0.0f;
	currentAnimationName_ = animationName;
	roopAnimation_ = roopAnimation;
}