#include <Common/System.h>
#include <Common/DataStructures/BitArray.h>
#include <Common/BLASingleton.h>
#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class InputManager;

    enum BLAKeyboard : blaU32
    {
        BLA_KEY_SPACE
        , BLA_KEY_APOSTROPHE
        , BLA_KEY_COMMA
        , BLA_KEY_MINUS
        , BLA_KEY_PERIOD
        , BLA_KEY_SLASH
        , BLA_KEY_0
        , BLA_KEY_1
        , BLA_KEY_2
        , BLA_KEY_3
        , BLA_KEY_4
        , BLA_KEY_5
        , BLA_KEY_6
        , BLA_KEY_7
        , BLA_KEY_8
        , BLA_KEY_9
        , BLA_KEY_SEMICOLON
        , BLA_KEY_EQUAL
        , BLA_KEY_A
        , BLA_KEY_B
        , BLA_KEY_C
        , BLA_KEY_D
        , BLA_KEY_E
        , BLA_KEY_F
        , BLA_KEY_G
        , BLA_KEY_H
        , BLA_KEY_I
        , BLA_KEY_J
        , BLA_KEY_K
        , BLA_KEY_L
        , BLA_KEY_M
        , BLA_KEY_N
        , BLA_KEY_O
        , BLA_KEY_P
        , BLA_KEY_Q
        , BLA_KEY_R
        , BLA_KEY_S
        , BLA_KEY_T
        , BLA_KEY_U
        , BLA_KEY_V
        , BLA_KEY_W
        , BLA_KEY_X
        , BLA_KEY_Y
        , BLA_KEY_Z
        , BLA_KEY_LEFT_BRACKET
        , BLA_KEY_BACKSLASH
        , BLA_KEY_RIGHT_BRACKET
        , BLA_KEY_GRAVE_ACCENT
        , BLA_KEY_WORLD_1
        , BLA_KEY_WORLD_2
        , BLA_KEY_ESCAPE
        , BLA_KEY_ENTER
        , BLA_KEY_TAB
        , BLA_KEY_BACKSPACE
        , BLA_KEY_INSERT
        , BLA_KEY_DELETE
        , BLA_KEY_RIGHT
        , BLA_KEY_LEFT
        , BLA_KEY_DOWN
        , BLA_KEY_UP
        , BLA_KEY_PAGE_UP
        , BLA_KEY_PAGE_DOWN
        , BLA_KEY_HOME
        , BLA_KEY_END
        , BLA_KEY_CAPS_LOCK
        , BLA_KEY_SCROLL_LOCK
        , BLA_KEY_NUM_LOCK
        , BLA_KEY_PRINT_SCREEN
        , BLA_KEY_PAUSE
        , BLA_KEY_F1
        , BLA_KEY_F2
        , BLA_KEY_F3
        , BLA_KEY_F4
        , BLA_KEY_F5
        , BLA_KEY_F6
        , BLA_KEY_F7
        , BLA_KEY_F8
        , BLA_KEY_F9
        , BLA_KEY_F10
        , BLA_KEY_F11
        , BLA_KEY_F12
        , BLA_KEY_F13
        , BLA_KEY_F14
        , BLA_KEY_F15
        , BLA_KEY_F16
        , BLA_KEY_F17
        , BLA_KEY_F18
        , BLA_KEY_F19
        , BLA_KEY_F20
        , BLA_KEY_F21
        , BLA_KEY_F22
        , BLA_KEY_F23
        , BLA_KEY_F24
        , BLA_KEY_F25
        , BLA_KEY_KP_0
        , BLA_KEY_KP_1
        , BLA_KEY_KP_2
        , BLA_KEY_KP_3
        , BLA_KEY_KP_4
        , BLA_KEY_KP_5
        , BLA_KEY_KP_6
        , BLA_KEY_KP_7
        , BLA_KEY_KP_8
        , BLA_KEY_KP_9
        , BLA_KEY_KP_DECIMAL
        , BLA_KEY_KP_DIVIDE
        , BLA_KEY_KP_MULTIPLY
        , BLA_KEY_KP_SUBTRACT
        , BLA_KEY_KP_ADD
        , BLA_KEY_KP_ENTER
        , BLA_KEY_KP_EQUAL
        , BLA_KEY_LEFT_SHIFT
        , BLA_KEY_LEFT_CONTROL
        , BLA_KEY_LEFT_ALT
        , BLA_KEY_LEFT_SUPER
        , BLA_KEY_RIGHT_SHIFT
        , BLA_KEY_RIGHT_CONTROL
        , BLA_KEY_RIGHT_ALT
        , BLA_KEY_RIGHT_SUPER
        , BLA_KEY_MENU
        , BLA_KEY_ENUM_END
    };

    enum BLAMouseButtons : blaU32
    {
        BLA_MOUSE_BUTTON_1
        , BLA_MOUSE_BUTTON_2
        , BLA_MOUSE_BUTTON_3
        , BLA_MOUSE_BUTTON_4
        , BLA_MOUSE_BUTTON_5
        , BLA_MOUSE_BUTTON_6
        , BLA_MOUSE_BUTTON_7
        , BLA_MOUSE_ENUM_END
        , BLA_MOUSE_BUTTON_LEFT = BLA_MOUSE_BUTTON_1
        , BLA_MOUSE_BUTTON_RIGHT = BLA_MOUSE_BUTTON_2
        , BLA_MOUSE_BUTTON_MIDDLE = BLA_MOUSE_BUTTON_3
    };

    enum BLAGamepadButtons : blaU32
    {
        BLA_GAMEPAD_DPAD_UP
        , BLA_GAMEPAD_DPAD_RIGHT
        , BLA_GAMEPAD_DPAD_DOWN
        , BLA_GAMEPAD_DPAD_LEFT
        , BLA_GAMEPAD_FACEBUTTON_UP
        , BLA_GAMEPAD_FACEBUTTON_RIGHT
        , BLA_GAMEPAD_FACEBUTTON_DOWN
        , BLA_GAMEPAD_FACEBUTTON_LEFT
        , BLA_GAMEPAD_LEFT_TRIGGER
        , BLA_GAMEPAD_LEFT_SHOULDER
        , BLA_GAMEPAD_LEFT_STICK
        , BLA_GAMEPAD_RIGHT_TRIGGER
        , BLA_GAMEPAD_RIGHT_SHOULDER
        , BLA_GAMEPAD_RIGHT_STICK
        , BLA_GAMEPAD_START
        , BLA_GAMEPAD_SELECT
        , BLA_GAMEPAD_ENUM_END
    };

    struct BLAButtonState
    {
        friend InputManager;
    protected:
        blaU8 m_state;

        BLAButtonState(blaU8 state) :
            m_state(state) {};
    public:

        blaBool IsDown() const { return m_state & 0x01; }
        blaBool IsUp() const { return !IsDown(); }

        blaBool IsRisingEdge() const { return m_state == 0x01; }
        blaBool IsFallingEdge() const { return m_state == 0x02; }
    };

    struct BLAButtonTimedState : BLAButtonState
    {
        friend InputManager;
    private:
        blaF32 m_time;

        BLAButtonTimedState(blaU8 state, blaF32 time) :
            BLAButtonState(state), m_time(time) {};
    public:

        blaF32 GetTimeOfLastEdge() const { return m_time; }
    };

    struct BLAMousePointerState
    {
        friend InputManager;
    private:
        blaVec2 m_position;
        blaVec2 m_deltaPosition;
    public:
        const blaVec2& GetPosition() const { return m_position; }
        const blaVec2& GetDelta() const { return m_deltaPosition; }
    };

    struct BLAMouseStateStorage
    {
        blaVec2 m_deltaMousePointerPosition = blaVec2(0.f);
        blaVec2 m_previousMousePointerPosition = blaVec2(0.f);
        blaVec2 m_mousePointerPosition = blaVec2(0.f);
        blaF32 m_scrollAxisDelta = 0.f;
    };

    struct BLAGamepadStateStorage
    {
        blaVec2 m_deltaLeftStickPosition = blaVec2(0.f);
        blaVec2 m_previousLeftStickPosition = blaVec2(0.f);
        blaVec2 m_leftStickPosition = blaVec2(0.f);

        blaVec2 m_deltaRightStickPosition = blaVec2(0.f);
        blaVec2 m_previousRightStickPosition = blaVec2(0.f);
        blaVec2 m_rightStickPosition = blaVec2(0.f);
    };

    typedef BLAButtonState BLAKeyState;
    typedef BLAButtonTimedState BLAKeyTimedState;


    typedef BLAButtonState BLAMouseState;
    typedef BLAButtonTimedState BLAMouseTimedState;


    typedef BLAButtonState BLAGamepadState;
    typedef BLAButtonTimedState BLAGamepadTimedState;

    typedef BLAMousePointerState BLAGamepadAnalogState;

    class InputStateSetter
    {
    public:
        static void SetKey(BLAKeyboard key, blaF32 time, blaBool down);
        static void SetMouseButton(BLAMouseButtons mouseButton, blaF32 time, blaBool down);
        static void SetGamepadButton(int index, BLAGamepadButtons pad, blaF32 time, blaBool down);

        static void SetMousePointer(const blaVec2& position);
        static void SetMouseScrollDelta(blaF32 mouseScrollDelta);

        static void SetGamepadSticks(const blaVec2& leftStick, const blaVec2& rightStick);
    };

    class InputManager
    {
        friend InputStateSetter;

        BLA_DECLARE_SINGLETON(InputManager)

    private:
        BitArray<BLAKeyboard::BLA_KEY_ENUM_END> m_keysSet;
        BitArray<BLAKeyboard::BLA_KEY_ENUM_END> m_previousKeysSet;

        BitArray<BLAMouseButtons::BLA_MOUSE_ENUM_END> m_mouseButtonSet;
        BitArray<BLAMouseButtons::BLA_MOUSE_ENUM_END> m_previousMouseButtonSet;

        BitArray<BLAGamepadButtons::BLA_GAMEPAD_ENUM_END> m_gamePadSet;
        BitArray<BLAGamepadButtons::BLA_GAMEPAD_ENUM_END> m_previousGamePadSet;

        blaF32 m_keyboardTimes[BLAKeyboard::BLA_KEY_ENUM_END];
        blaF32 m_mouseButtonTimes[BLAMouseButtons::BLA_MOUSE_ENUM_END];
        blaF32 m_gamepadTimes[BLAGamepadButtons::BLA_GAMEPAD_ENUM_END];

        BLAMouseStateStorage m_mouseState;
        BLAGamepadStateStorage m_gamepadState;

        blaBool m_lockInputs;

    public:

        InputManager() :
            m_lockInputs(false)
            , m_lockMouse(false)
            , m_lockKeyboard(false)
        {
            for (blaU32 i = 0; i < BLAKeyboard::BLA_KEY_ENUM_END; ++i)
            {
                m_keyboardTimes[i] = 0.f;
            }

            for (blaU32 i = 0; i < BLAMouseButtons::BLA_MOUSE_ENUM_END; ++i)
            {
                m_mouseButtonTimes[i] = 0.f;
            }

            for (blaU32 i = 0; i < BLAGamepadButtons::BLA_GAMEPAD_ENUM_END; ++i)
            {
                m_gamepadTimes[i] = 0.f;
            }
        };

        blaBool m_lockMouse, m_lockKeyboard;

        BLAKeyState GetKeyState(BLAKeyboard key) const;
        BLAKeyTimedState GetKeyTimedState(BLAKeyboard key) const;

        BLAMouseState GetMouseButtonState(BLAMouseButtons mouseButton) const;
        BLAMouseTimedState GetMouseButtonTimedState(BLAMouseButtons mouseButton) const;

        BLAMousePointerState GetMousePointerState() const;

        blaF32 GetMouseScrollDelta() const { return m_mouseState.m_scrollAxisDelta; };

        BLAGamepadAnalogState GetGamepadLeftAnalog() const;
        BLAGamepadAnalogState GetGamepadRightAnalog() const;

        BLAGamepadState GetGamepadState(BLAGamepadButtons padButton) const;
        BLAGamepadTimedState GetGamepadTimedState(BLAGamepadButtons padButton) const;

        void Update();
    };
}