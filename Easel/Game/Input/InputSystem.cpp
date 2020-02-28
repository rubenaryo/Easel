/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : InputSystem method definitions
----------------------------------------------*/
#include "InputSystem.h"

namespace Input {

    // Init all keyboard states to 0
    InputSystem::InputSystem()
    {
        m_KeyboardCurrent.fill(0);
        m_KeyboardPrevious.fill(0);
        m_MousePrevious.x = 0;
        m_MousePrevious.y = 0;
        m_MouseCurrent.x = 0;
        m_MouseCurrent.y = 0;
    }

    // Release all dynamic memory
    InputSystem::~InputSystem()
    {
        // Release keyMap and clear
        for (auto pair : m_keyMap)
            delete pair.second;
        m_keyMap.clear();

        // No need to delete dynamic memory from activeKeyMap, it's already deleted in keymap*
        m_activeKeyMap.clear();
    }

    // Stores previous/current keymappings (from windows)
    // Updates active keymap by comparing all chords 
    // Updates mouse mapping
    void InputSystem::GetInput()
    {
        // Get the keyboard state from windows
        GetKeyboardState();

        // Update active/non-active keymaps
        update();

        // Update mouse state
        m_MousePrevious = m_MouseCurrent;
    }

    void InputSystem::OnMouseMove(short newX, short newY)
    {
        m_MouseCurrent = { newX, newY };
    }

    std::pair<float,float> InputSystem::GetMouseDelta() const
    {
        std::pair<float, float> pt;
        
        pt.first  = static_cast<float>(m_MouseCurrent.x - m_MousePrevious.x);
        pt.second = static_cast<float>(m_MouseCurrent.y - m_MousePrevious.y);
        return pt;
    }

    // Clears active key map, then fills it with all values from m_keyMap with a 'fulfilled' chord
    void InputSystem::update()
    {
        // Reset active key map
        m_activeKeyMap.clear();

        // Map which keys are active into the active key map
        for (auto key : m_keyMap)
        {
            bool activeKey = true;

            // Test Chord
            for (Binding binding : key.second->GetChord())
            {
                if (GetKeyboardKeyState(binding.m_KeyCode) != binding.m_KeyState)
                {
                    activeKey = false;
                    break;
                }
            }

            // Passed Chord Check : move key to active key map
            if (activeKey)
                m_activeKeyMap.insert(std::pair<GameCommands, Chord*>(key.first, key.second));
        }
    }

    // Stores the current state of the keyboard as 'previous'
    // then reads in new 'current values from windows
    void InputSystem::GetKeyboardState()
    {
        m_KeyboardPrevious = m_KeyboardCurrent;

        for (int i = 0; i < 256; i++)
            m_KeyboardCurrent[i] = isPressed(i);
    }

    // Use logic to deduce Keystate from current and previous keyboard states
    const KeyState InputSystem::GetKeyboardKeyState(const unsigned int a_keyCode) const
    {
        if (m_KeyboardPrevious[a_keyCode] == 1)
            if (m_KeyboardCurrent[a_keyCode] == 1)
                return KeyState::StillPressed;  // true, true
            else
                return KeyState::JustReleased;  // true, false
        else
            if (m_KeyboardCurrent[a_keyCode] == 1)
                return KeyState::JustPressed;   // false, true
            else
                return KeyState::StillReleased; // false, false
    }
}