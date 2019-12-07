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
        m_KeyCode(0),
        m_KeyState(KeyState::JustReleased)
    {};

    // Create Binding from keycode and keystate
    Binding::Binding(const unsigned int a_keyCode, const KeyState a_keyState) :
        m_KeyCode(a_keyCode),
        m_KeyState(a_keyState)
    {};

    // Default Chord Constructor
    Chord::Chord() :
        m_Name(L""),
        m_Chord(0)
    {};

    // Create Chord from Name, create binding from keycode and keystate and add it to Chord structure
    Chord::Chord(const std::wstring& a_Name, const unsigned int a_keyCode, const KeyState a_keyState) :
        m_Name(a_Name)
    {
        m_Chord.push_back(Binding(a_keyCode, a_keyState));
    }

    // Create Chord from Name and Binding&
    Chord::Chord(const std::wstring& a_Name, const Binding& a_Binding) :
        m_Name(a_Name)
    {
        m_Chord.push_back(a_Binding);
    }

    // Create Chord from Name and vector of Bindings
    Chord::Chord(const std::wstring& a_Name, const std::vector<Binding>& a_lBinding) :
        m_Name(a_Name),
        m_Chord(a_lBinding)
    {};
}
