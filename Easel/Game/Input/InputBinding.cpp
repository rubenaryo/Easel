/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : Implementation for structures defined in InputBinding.h
----------------------------------------------*/
#include "InputBinding.h"
#include <vector>
#include <string>

namespace Input {
    // Default Binding Constructor
    Binding::Binding() :
        mKeyCode(0),
        mKeyState(KeyState::JustReleased)
    {};

    // Create Binding from keycode and keystate
    Binding::Binding(const unsigned int a_keyCode, const KeyState a_keyState) :
        mKeyCode(a_keyCode),
        mKeyState(a_keyState)
    {};

    // Default Chord Constructor
    Chord::Chord() :
        mName(L""),
        mChord(0)
    {};

    // Create Chord from Name, create binding from keycode and keystate and add it to Chord structure
    Chord::Chord(const std::wstring& a_Name, const unsigned int a_keyCode, const KeyState a_keyState) :
        mName(a_Name)
    {
        mChord.push_back(Binding(a_keyCode, a_keyState));
    }

    // Create Chord from Name and Binding&
    Chord::Chord(const std::wstring& a_Name, const Binding& a_Binding) :
        mName(a_Name)
    {
        mChord.push_back(a_Binding);
    }

    // Create Chord from Name and vector of Bindings
    Chord::Chord(const std::wstring& a_Name, const std::vector<Binding>& a_lBinding) :
        mName(a_Name),
        mChord(a_lBinding)
    {};
}
