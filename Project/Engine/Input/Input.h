#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/ComPtr.h>
#include <Engine/Input/InputStructures.h>
#include <Lib/MathUtils/Vector2.h>

// directInput
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <XInput.h>
// c++
#include <cmath>
#include <cstdint>
#include <array>
#include <cassert>
// front
class WinApp;

//============================================================================
//	Input class
//============================================================================
class Input {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(WinApp* winApp);
	void Update();

	//--------- accessor -----------------------------------------------------

	// key
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);
	bool ReleaseKey(BYTE keyNumber);

	// gamePad
	bool PushGamepadButton(GamePadButtons button);
	bool TriggerGamepadButton(GamePadButtons button);

	Vector2 GetLeftStickVal() const;
	Vector2 GetRightStickVal() const;

	float GetLeftTriggerValue() const;
	float GetRightTriggerValue() const;

	// mouse
	bool PushMouseLeft() const;
	bool PushMouseRight() const;
	bool PushMouseCenter() const;

	bool TriggerMouseLeft() const;
	bool TriggerMouseRight() const;
	bool TriggerMouseCenter() const;

	Vector2 GetMousePos() const;
	Vector2 GetMousePrePos() const;
	Vector2 GetMouseMoveValue() const;
	float GetMouseWheel();

	InputType GetType() const { return inputType_; }

	// deadZone
	void SetDeadZone(float deadZone);

	// singleton
	static Input* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static Input* instance_;

	WinApp* winApp_;

	// 入力状態
	InputType inputType_;

	// key
	std::array<BYTE, 256> key_{};
	std::array<BYTE, 256> keyPre_{};

	ComPtr<IDirectInput8> dInput_;
	ComPtr<IDirectInputDevice8> keyboard_;

	// gamePad
	XINPUT_STATE gamepadState_{};
	XINPUT_STATE gamepadStatePre_{};

	std::array<bool, static_cast<size_t>(GamePadButtons::Counts)> gamepadButtons_{};
	std::array<bool, static_cast<size_t>(GamePadButtons::Counts)> gamepadButtonsPre_{};

	float leftThumbX_;
	float leftThumbY_;
	float rightThumbX_;
	float rightThumbY_;

	// デッドゾーンの閾値
	float deadZone_ = 9600.0f;

	// LTボタン
	float leftTriggerValue_ = 0.0f;
	// RTボタン
	float rightTriggerValue_ = 0.0f;

	// mouse
	DIMOUSESTATE mouseState_;

	ComPtr<IDirectInputDevice8> mouse_; // マウスデバイス

	std::array<bool, 3> mouseButtons_;    // マウスボタンの状態
	std::array<bool, 3> mousePreButtons_; // 1フレ前のマウスボタンの状態
	Vector2 mousePos_;                    // マウスの座標
	Vector2 mousePrePos_;                 // マウスの前座標
	float wheelValue_;                    // ホイール移動量

	//--------- functions ----------------------------------------------------

	float ApplyDeadZone(float value);

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
};