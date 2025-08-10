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
#include <string_view>
#include <cstdint>
#include <array>
#include <cassert>
#include <source_location>
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
	bool PushKey(BYTE keyNumber, const std::source_location& location = std::source_location::current());
	bool TriggerKey(BYTE keyNumber, const std::source_location& location = std::source_location::current());
	bool ReleaseKey(BYTE keyNumber, const std::source_location& location = std::source_location::current());

	// gamePad
	bool PushGamepadButton(GamePadButtons button, const std::source_location& location = std::source_location::current());
	bool TriggerGamepadButton(GamePadButtons button, const std::source_location& location = std::source_location::current());

	Vector2 GetLeftStickVal() const;
	Vector2 GetRightStickVal() const;

	float GetLeftTriggerValue() const;
	float GetRightTriggerValue() const;

	// mouse
	bool PushMouseLeft(const std::source_location& location = std::source_location::current()) const { return PushMouseButton(0, location); }
	bool PushMouseRight(const std::source_location& location = std::source_location::current()) const { return PushMouseButton(1, location); }
	bool PushMouseCenter(const std::source_location& location = std::source_location::current()) const { return PushMouseButton(2, location); }
	bool PushMouse(MouseButton button, const std::source_location& location = std::source_location::current()) const;

	bool TriggerMouseLeft(const std::source_location& location = std::source_location::current()) const;
	bool TriggerMouseRight(const std::source_location& location = std::source_location::current()) const;
	bool TriggerMouseCenter(const std::source_location& location = std::source_location::current()) const;
	bool TriggerMouse(MouseButton button, const std::source_location& location = std::source_location::current()) const;

	bool ReleaseMouse(MouseButton button, const std::source_location& location = std::source_location::current()) const;

	Vector2 GetMousePos() const;
	Vector2 GetMousePrePos() const;
	Vector2 GetMouseMoveValue() const;
	float GetMouseWheel();

	InputType GetType() const { return inputType_; }

	// deadZone
	void SetDeadZone(float deadZone);
	float GetDeadZone() const { return deadZone_; }

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

	std::array<std::chrono::steady_clock::time_point, 256> keyStartTime_{};
	std::array<bool, 256> keyStayLogged_{};

	std::array<std::chrono::steady_clock::time_point, static_cast<size_t>(GamePadButtons::Counts)> gpStartTime_{};
	std::array<bool, static_cast<size_t>(GamePadButtons::Counts)> gpStayLogged_{};

	std::array<std::chrono::steady_clock::time_point, 3> mouseStartTime_{};
	std::array<bool, 3> mouseStayLogged_{};

	float leftThumbX_;
	float leftThumbY_;
	float rightThumbX_;
	float rightThumbY_;

	// デッドゾーンの閾値
	float deadZone_ = 16000.0f;

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

	// helper
	bool PushMouseButton(size_t index, const std::source_location& location) const;
	float ApplyDeadZone(float value);

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
};