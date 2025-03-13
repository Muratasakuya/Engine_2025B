#include "TemplateObject3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	teapotId_ = ComponentManager::GetInstance()->CreateObject3D("teapot", std::nullopt, "teapot");
}

void TemplateObject3D::Update() {


}