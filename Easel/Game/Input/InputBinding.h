/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Various class/struct definitions 
to be used by the Inputhandling InputSystem class
----------------------------------------------*/
#ifndef INPUTBINDING_H
#define INPUTBINDING_H

// Preset our windows includes
#include "../../System/WinApp.h"

// std inclusions
#include <string>
#include <array>
#include <vector>

namespace Input {
    
    // Enumeration of different GameCommands
    enum class GameCommands
    {
        Quit,
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight
    };

    // Enum to emphasize the different states of a key
    enum class KeyState
    {
        StillReleased,
        JustPressed,
        StillPressed,
        JustReleased
    };

    // Wrapping struct keycode and above enum
    struct Binding
    {
    private:
        unsigned int m_KeyCode;         // Windows Keycode
        KeyState m_KeyState;            // Associated keystate

    public:
        Binding();
        Binding(const unsigned int a_keyCode, const KeyState a_keyState);
        ~Binding() {};

        friend class InputSystem;
    };

    // Maps a game command to a Binding
    struct Chord
    {
    private:
        std::wstring m_Name;            // Human readable command name
        std::vector<Binding> m_Chord;   // Sequence of keys mapped to "m_Name" command

    public:
        Chord();
        Chord(const std::wstring& a_Name, const unsigned int a_KeyCode, const KeyState a_KeyState);
        Chord(const std::wstring& a_Name, const Binding& a_Chord);
        Chord(const std::wstring& a_Name, const std::vector<Binding>& a_lChord);
        ~Chord() {};

        // Accessors for member variables
        std::vector<Binding>& GetChord() { return m_Chord; }
        std::wstring&         GetName()  { return m_Name;  }
    };
}

#endif