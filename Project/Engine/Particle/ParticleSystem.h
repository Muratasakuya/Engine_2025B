#pragma once

//============================================================================
//	include
//============================================================================

// front
class CameraManager;

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(CameraManager* cameraManager);

	void Update();

	//--------- accessor -----------------------------------------------------

	// singleton
	static ParticleSystem* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ParticleSystem* instance_;

	CameraManager* cameraManager_;

	//--------- functions ----------------------------------------------------

	ParticleSystem() = default;
	~ParticleSystem() = default;
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;
};