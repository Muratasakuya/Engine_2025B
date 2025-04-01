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

	inline static constexpr const float itemWidth_ = 192.0f;
}// namespace

void DissolveForGPU::ImGui() {

	ImGui::Text("Dissolve");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##Dissolve", &threshold, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("edgeSize##Dissolve", &edgeSize, 0.01f, 0.0f, 4.0f);
	ImGui::ColorEdit3("edgeColor##Dissolve", &edgeColor.x);
	ImGui::ColorEdit3("thresholdColor##Dissolve", &thresholdColor.x);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void GaussianFilterForGPU::ImGui() {

	ImGui::Text("GaussianFilter");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("sigma##GaussianFilter", &sigma, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void LuminanceBasedOutlineForGPU::ImGui() {

	ImGui::Text("LuminanceBasedOutline");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("strength##LuminanceBasedOutline", &strength, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void DepthBasedOutlineForGPU::ImGui() {
}

void VignetteForGPU::ImGui() {

	ImGui::Text("Vignette");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("scale##Vignette", &scale, 0.01f);
	ImGui::DragFloat("power##Vignette", &power, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void RadialBlurForGPU::ImGui() {

	ImGui::Text("RadialBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat2("center##RadialBlur", &center.x, 0.01f);
	ImGui::DragInt("scale##RadialBlur", &numSamples);
	ImGui::DragFloat("power##RadialBlur", &width, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void HorizonBlurForGPU::ImGui() {

	ImGui::Text("HorizonBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##HorizonBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##HorizonBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void VerticalBlurForGPU::ImGui() {

	ImGui::Text("VerticalBlur");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragInt("radius##VerticalBlur", &radius, 1, 0, 48);
	ImGui::DragFloat("sigma##VerticalBlur", &sigma, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}

void LuminanceExtractForGPU::ImGui() {

	ImGui::Text("BloomLuminanceExtract");

	ImGui::PushItemWidth(itemWidth_);
	ImGui::DragFloat("threshold##BloomLuminanceExtract", &threshold, 0.01f);
	ImGui::PopItemWidth();

	ImGui::Separator();
}