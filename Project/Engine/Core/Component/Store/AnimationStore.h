#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/AnimationComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>

// skinning設計
// skeleton、skinClusterはcomponentごとに必要
// animationDataは読み込んだ時点のデータ1つでいい
// CreateObject3Dした時にskinClusterのbuffer(palette、influence)、IO頂点bufferを作成する(bufferSize * インスタンス数分)
// 各componentにはskeleton、skinClusterを持たせて各々更新できるようにする
// dispatch処理はInstancedMeshにやらせる(予定)

//============================================================================
//	AnimationStore class
//============================================================================
class AnimationStore :
	public IComponentStore<AnimationComponent> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationStore() = default;
	~AnimationStore() = default;

	void AddComponent(uint32_t entityId, std::any args) override;
	void RemoveComponent(uint32_t entityId) override;

	void Update() override;

	//--------- accessor -----------------------------------------------------

	AnimationComponent* GetComponent(uint32_t entityId) override;
	std::vector<AnimationComponent*> GetComponentList([[maybe_unused]] uint32_t entityId) override { return { nullptr }; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<std::unique_ptr<AnimationComponent>> components_;
};