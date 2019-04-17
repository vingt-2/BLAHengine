#include "InputManager.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(InputManager)

BLAKeyState InputManager::GetKeyState(BLAKeyboard key) const
{
    return BLAKeyState(0x00 | (m_keysSet.IsBitSet(key) ? 0x01 : 0x00) | (m_previousKeysSet.IsBitSet(key) ? 0x02 : 0x00));
}

BLAKeyTimedState InputManager::GetKeyTimedState(BLAKeyboard key) const
{
    return BLAKeyTimedState(0x00 | (m_keysSet.IsBitSet(key) ? 0x01 : 0x00) | (m_previousKeysSet.IsBitSet(key) ? 0x02 : 0x00), m_keyboardTimes[key]);
}

BLAMouseState InputManager::GetMouseButtonState(BLAMouseButtons mouseButton) const
{
    return BLAMouseState(0x00 | (m_mouseButtonSet.IsBitSet(mouseButton) ? 0x01 : 0x00) | (m_previousMouseButtonSet.IsBitSet(mouseButton) ? 0x02 : 0x00));
}
BLAMouseTimedState InputManager::GetMouseButtonTimedState(BLAMouseButtons mouseButton) const
{
    return BLAMouseTimedState(0x00 | (m_mouseButtonSet.IsBitSet(mouseButton) ? 0x01 : 0x00) | (m_previousMouseButtonSet.IsBitSet(mouseButton) ? 0x02 : 0x00), m_mouseButtonTimes[mouseButton]);
}

BLAGamepadState InputManager::GetGamepadState(BLAGamepadButtons padButton) const
{
    return BLAGamepadState(0x00 | (m_gamePadSet.IsBitSet(padButton) ? 0x01 : 0x00) | (m_previousGamePadSet.IsBitSet(padButton) ? 0x02 : 0x00));
}

BLAGamepadTimedState InputManager::GetGamepadTimedState(BLAGamepadButtons padButton) const
{
    return BLAGamepadTimedState(0x00 | (m_gamePadSet.IsBitSet(padButton) ? 0x01 : 0x00) | (m_previousGamePadSet.IsBitSet(padButton) ? 0x02 : 0x00), m_gamepadTimes[padButton]);
}

BLAMousePointerState InputManager::GetMousePointerState() const
{
    BLAMousePointerState state;
    state.m_position = m_mouseState.m_mousePointerPosition;
    state.m_deltaPosition = m_mouseState.m_deltaMousePointerPosition;
    return state;
}

BLAGamepadAnalogState InputManager::GetGamepadLeftAnalog() const 
{
    BLAGamepadAnalogState state;
    state.m_position = m_gamepadState.m_leftStickPosition;
    state.m_deltaPosition = m_gamepadState.m_deltaLeftStickPosition;
    return state;
}

BLAGamepadAnalogState InputManager::GetGamepadRightAnalog() const
{
    BLAGamepadAnalogState state;
    state.m_position = m_gamepadState.m_rightStickPosition;
    state.m_deltaPosition = m_gamepadState.m_deltaRightStickPosition;
    return state;
}

void InputManager::Update()
{
    m_lockInputs = true;
    
    m_previousKeysSet = m_keysSet;
    m_previousMouseButtonSet = m_mouseButtonSet;
    m_previousGamePadSet = m_gamePadSet;

    m_mouseState.m_deltaMousePointerPosition = m_mouseState.m_mousePointerPosition - m_mouseState.m_previousMousePointerPosition;
    m_mouseState.m_previousMousePointerPosition = m_mouseState.m_mousePointerPosition;
    m_mouseState.m_scrollAxisDelta = 0.f;

    m_gamepadState.m_deltaLeftStickPosition = m_gamepadState.m_leftStickPosition - m_gamepadState.m_previousLeftStickPosition;
    m_gamepadState.m_previousLeftStickPosition = m_gamepadState.m_leftStickPosition;
    m_gamepadState.m_deltaRightStickPosition = m_gamepadState.m_rightStickPosition - m_gamepadState.m_previousRightStickPosition;
    m_gamepadState.m_previousRightStickPosition = m_gamepadState.m_rightStickPosition;
   
    m_lockInputs = false;
}

void InputStateSetter::SetKey(BLAKeyboard key, blaF32 time, blaBool down)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    while (inputManager->m_lockInputs);

    if (down)
        inputManager->m_keysSet.Set(key);
    else
        inputManager->m_keysSet.Clear(key);

    inputManager->m_keyboardTimes[key] = time;
}

void InputStateSetter::SetMouseButton(BLAMouseButtons key, blaF32 time, blaBool down)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    if (inputManager->m_lockMouse)
    {
        return;
    }

    while (inputManager->m_lockInputs);

    if (down)
        inputManager->m_mouseButtonSet.Set(key);
    else
        inputManager->m_mouseButtonSet.Clear(key);

    inputManager->m_mouseButtonTimes[key] = time;
}

void InputStateSetter::SetGamepadButton(int index, BLAGamepadButtons key, blaF32 time, blaBool down)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    while (inputManager->m_lockInputs);

    if (down)
        inputManager->m_gamePadSet.Set(key);
    else
        inputManager->m_gamePadSet.Clear(key);

    inputManager->m_gamepadTimes[key] = time;
}

void InputStateSetter::SetMousePointer(const blaVec2& position)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    if (inputManager->m_lockMouse)
    {
        return;
    }

    while (inputManager->m_lockInputs);

    inputManager->m_mouseState.m_mousePointerPosition = position;
}

void InputStateSetter::SetMouseScrollDelta(blaF32 mouseScrollDelta)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    if (inputManager->m_lockMouse)
    {
        return;
    }

    while (inputManager->m_lockInputs);

    inputManager->m_mouseState.m_scrollAxisDelta = mouseScrollDelta;
}

void InputStateSetter::SetGamepadSticks(const blaVec2& leftStick, const blaVec2& rightStick)
{
    InputManager* inputManager = InputManager::GetSingletonInstance();

    if (inputManager->m_lockMouse)
    {
        return;
    }

    while (inputManager->m_lockInputs);

    inputManager->m_gamepadState.m_leftStickPosition = leftStick;
    inputManager->m_gamepadState.m_rightStickPosition = rightStick;
}