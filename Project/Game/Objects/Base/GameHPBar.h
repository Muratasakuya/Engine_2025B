#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity2D.h>

//============================================================================
//	GameHPBar class
//============================================================================
class GameHPBar :
	public GameEntity2D {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameHPBar() = default;
	~GameHPBar() = default;

	void Init(const std::string& textureName, const std::string& alphaTextureName,
		const std::string& name, const std::string& groupName);

	void Update(int currentHP, int maxHP);
};