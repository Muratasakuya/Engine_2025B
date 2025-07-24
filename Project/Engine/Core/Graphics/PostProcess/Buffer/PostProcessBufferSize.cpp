#include "PostProcessBufferSize.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	PostProcessBufferSize classMethods
//============================================================================

namespace {

	inline static constexpr const float itemWidth_ = 224.0f;
}// namespace

void BloomForGPU::ImGui() {

	ImGui::SeparatorText("Bloom");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##BloomForGPU", &threshold, 0.01f);
	ImGui::DragInt("radius##BloomForGPU", &radius, 1, 0, 8);
	ImGui::DragFloat("sigma##BloomForGPU", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void DissolveForGPU::ImGui() {

	ImGui::SeparatorText("Dissolve");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##Dissolve", &threshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("edgeSize##Dissolve", &edgeSize, 0.01f, 0.0f, 4.0f);
	ImGui::ColorEdit3("edgeColor##Dissolve", &edgeColor.x);
	ImGui::ColorEdit3("thresholdColor##Dissolve", &thresholdColor.x);
	ImGui::PopItemWidth();
}

void GaussianFilterForGPU::ImGui() {

	ImGui::SeparatorText("GaussianFilter");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("sigma##GaussianFilter", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void LuminanceBasedOutlineForGPU::ImGui() {

	ImGui::SeparatorText("LuminanceBasedOutline");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("strength##LuminanceBasedOutline", &strength, 0.01f);
	ImGui::PopItemWidth();
}

void DepthBasedOutlineForGPU::ImGui() {

	ImGui::SeparatorText("DepthBasedOutline");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("strength##DepthBasedOutline", &edgeScale, 0.01f);
	ImGui::DragFloat("threshold##DepthBasedOutline", &threshold, 0.001f);
	ImGui::ColorEdit3("color##DepthBasedOutline", &color.x);
	ImGui::PopItemWidth();
}

void VignetteForGPU::ImGui() {

	ImGui::SeparatorText("Vignette");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("scale##Vignette", &scale, 0.01f);
	ImGui::DragFloat("power##Vignette", &power, 0.01f);
	ImGui::ColorEdit3("color##Vignette", &color.x);
	ImGui::PopItemWidth();
}

void RadialBlurForGPU::ImGui() {

	ImGui::SeparatorText("RadialBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat2("center##RadialBlur", &center.x, 0.01f);
	ImGui::DragInt("numSamples##RadialBlur", &numSamples);
	ImGui::DragFloat("width##RadialBlur", &width, 0.001f);
	ImGui::PopItemWidth();
}

void HorizonBlurForGPU::ImGui() {

	ImGui::SeparatorText("HorizonBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##HorizonBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##HorizonBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void HorizonBlurForGPU::ImGuiWithBloom() {

	ImGui::Text("HorizonBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##HorizonBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##HorizonBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void VerticalBlurForGPU::ImGui() {

	ImGui::SeparatorText("VerticalBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##VerticalBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##VerticalBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void VerticalBlurForGPU::ImGuiWithBloom() {

	ImGui::Text("VerticalBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##VerticalBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##VerticalBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();
}

void LuminanceExtractForGPU::ImGui() {

	ImGui::SeparatorText("BloomLuminanceExtract");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##BloomLuminanceExtract", &threshold, 0.01f);
	ImGui::PopItemWidth();
}

void LuminanceExtractForGPU::ImGuiWithBloom() {

	ImGui::Text("BloomLuminanceExtract");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##BloomLuminanceExtract", &threshold, 0.01f);
	ImGui::PopItemWidth();
}

void LutForGPU::ImGui() {

	ImGui::Text("Lut");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("lerpRate##Lut", &lerpRate, 0.001f, 0.0f, 1.0f);
	ImGui::PopItemWidth();
}

void RandomForGPU::ImGui() {

	ImGui::Text("Random");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("time##Random", &time, 0.001f);
	ImGui::PopItemWidth();
}