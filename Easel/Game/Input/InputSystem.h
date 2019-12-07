/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Interface for the InputSystem class
----------------------------------------------*/
#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "../../System/WinApp.h"
#include <array>
#include <unordered_map>
#include "InputBinding.h"

namespace Input {
    
    class InputSystem
    {
    private:
        // Keyboard States
        std::array<BYTE, 256> m_KeyboardCurrent;
        std::array<BYTE, 256> m_KeyboardPrevious;

        // Mouse States
        POINTS m_MouseCurrent;
        POINTS m_MousePrevious;

        // Uses GetAsyncKeyState to read in 256 bytes
        void GetKeyboardState();

        // returns the state of the key in enum form
        const KeyState GetKeyboardKeyState(const unsigned int a_keyCode) const;

        // returns true if the key is down
        inline const bool isPressed(int a_keyCode) const
        {
            return (GetAsyncKeyState(a_keyCode) & 0x8000) ? 1 : 0;
        }

        void update();

    protected:
        std::unordered_map<GameCommands, Chord*> m_keyMap;

        virtual void SetDefaultKeyMap() = 0;

    public:
        InputSystem();
        virtual ~InputSystem();

        std::unordered_map<GameCommands, Chord*> m_activeKeyMap;

        // Main "Update method" for input system
        void GetInput();

        // On WM_MOUSEMOVE message, trigger this method
        void OnMouseMove(POINTS mousePos);

        // Returns the current mouse position as a POINT
        POINTS GetMousePosition() const { return m_MouseCurrent; }

        // Returns the difference between current and previous as a POINT
        POINTS GetMouseDelta() const;

    };
}
#endif