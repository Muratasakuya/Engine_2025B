#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Base/GameObject2D.h>

//============================================================================
//	GameHPBar class
//============================================================================
class GameHPBar :
	public GameObject2D {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameHPBar() = default;
	~GameHPBar() = default;

	void Init(const std::string& textureName, const std::string& alphaTextureName,
		const std::string& name, const std::string& groupName);

	void Update(int current, int max, bool isReverse);
};