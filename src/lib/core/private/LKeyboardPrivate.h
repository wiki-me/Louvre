#ifndef LKEYBOARDPRIVATE_H
#define LKEYBOARDPRIVATE_H

#include <private/LCompositorPrivate.h>
#include <private/LSeatPrivate.h>
#include <LKeyboard.h>
#include <vector>

using namespace Louvre;
using namespace Louvre::Protocols;

struct LKeyboard::Params
{
    /* Add here any required constructor param */
};

LPRIVATE_CLASS(LKeyboard)

    LSurface *keyboardFocusSurface = nullptr;

    // XKB
    xkb_context *xkbContext = nullptr;
    xkb_keymap *xkbKeymap = nullptr;
    xkb_state *xkbKeymapState = nullptr;
    xkb_rule_names xkbKeymapName;
    Int32 xkbKeymapSize;
    Int32 xkbKeymapFd = -1;
    UInt32 keymapFormat;

    LKeyboardModifiersEvent::Modifiers currentModifiersState;
    LKeyboardModifiersEvent::Modifiers prevModifiersState;
    bool modifiersChanged = true;

    std::list<UInt32>pressedKeys;

    // Since 4
    Int32 repeatRate = 32;
    Int32 repeatDelay = 500;

    // Grab
    LSurface *grabbingSurface = nullptr;
    Wayland::RKeyboard *grabbingKeyboardResource = nullptr;
};

#endif // LKEYBOARDPRIVATE_H
