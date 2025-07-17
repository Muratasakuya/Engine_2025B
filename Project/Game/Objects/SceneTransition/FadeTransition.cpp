//#include "FadeTransition.h"
//
////============================================================================
////	include
////============================================================================
//#include <Engine/Core/Window/WinApp.h>
//
////============================================================================
////	FadeTransition classMethods
////============================================================================
//
//void FadeTransition::Init() {
//
//	// fade用のsprite
//	fadeSpriteId_ = GameObjectHelper::CreateObject2D("white", "fadeTransition", "SceneTransition");
//	// component取得
//	Transform2D* fadeTransform = Component::GetData<Transform2D>(fadeSpriteId_);
//	SpriteMaterial* fadeMaterial = Component::GetData<SpriteMaterial>(fadeSpriteId_);
//
//	// windowSize
//	Vector2 windowSize = Vector2(static_cast<float>(WinApp::GetWindowWidth()),
//		static_cast<float>(WinApp::GetWindowHeight()));
//
//	// fade設定
//	fadeTransform->SetCenterPos();
//	fadeTransform->size = windowSize;
//	fadeMaterial->color.a = 0.0f;
//
//	// timer設定
//	beginTimer_.target_ = 1.0f;
//	waitTimer_.target_ = 0.5f;
//	endTimer_.target_ = 1.0f;
//
//	beginTimer_.easeingType_ = EasingType::EaseInSine;
//	waitTimer_.easeingType_ = EasingType::EaseInSine;
//	endTimer_.easeingType_ = EasingType::EaseInSine;
//
//	beginTimer_.Reset();
//	waitTimer_.Reset();
//	endTimer_.Reset();
//}
//
//void FadeTransition::Update() {}
//
//void FadeTransition::BeginUpdate() {
//
//	// component取得
//	SpriteMaterial* fadeMaterial = Component::GetData<SpriteMaterial>(fadeSpriteId_);
//
//	beginTimer_.Update();
//	fadeMaterial->color.a = std::lerp(0.0f, 1.0f, beginTimer_.easedT_);
//
//	if (beginTimer_.IsReached()) {
//
//		fadeMaterial->color.a = 1.0f;
//
//		// 次に進める
//		state_ = TransitionState::Wait;
//		beginTimer_.Reset();
//	}
//}
//
//void FadeTransition::WaitUpdate() {
//
//	waitTimer_.Update();
//
//	if (waitTimer_.IsReached()) {
//
//		// 次に進める
//		state_ = TransitionState::End;
//		waitTimer_.Reset();
//	}
//}
//
//void FadeTransition::EndUpdate() {
//
//	// component取得
//	SpriteMaterial* fadeMaterial = Component::GetData<SpriteMaterial>(fadeSpriteId_);
//
//	endTimer_.Update();
//	fadeMaterial->color.a = std::lerp(1.0f, 0.0f, endTimer_.easedT_);
//
//	if (endTimer_.IsReached()) {
//
//		fadeMaterial->color.a = 0.0f;
//
//		// 遷移終了
//		state_ = TransitionState::Begin;
//		endTimer_.Reset();
//	}
//}
//
//void FadeTransition::ImGui() {
//
//	ImGui::Text("beginTransitionT: %4.3f", beginTimer_.t_);
//	ImGui::Text("beginTransitionEasedT: %4.3f", beginTimer_.easedT_);
//
//	ImGui::Text("waitT: %4.3f", waitTimer_.t_);
//	ImGui::Text("waitEasedT: %4.3f", waitTimer_.easedT_);
//
//	ImGui::Text("endTransitionT: %4.3f", endTimer_.t_);
//	ImGui::Text("endTransitionEasedT: %4.3f", endTimer_.easedT_);
//}