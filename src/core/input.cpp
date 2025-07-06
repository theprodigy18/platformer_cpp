#include "core/input.hpp"

namespace drop::core
{

    Input*    g_input;
    KeyCodeID KeyCodeLookupTable[KEY_COUNT];

    bool KeyPressedThisFrame(KeyCodeID keyCode)
    {
        Key  key {g_input->keys[keyCode]};
        bool result {key.isDown && key.halfTransitionCount == 1 || key.halfTransitionCount > 1};
        return result;
    }

    bool KeyReleasedThisFrame(KeyCodeID keyCode)
    {
        Key  key {g_input->keys[keyCode]};
        bool result {!key.isDown && key.halfTransitionCount == 1 || key.halfTransitionCount > 1};
        return result;
    }

    bool KeyIsDown(KeyCodeID keyCode)
    {
        return g_input->keys[keyCode].isDown;
    }
} // namespace drop::shared
