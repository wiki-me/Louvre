#ifndef LPOINTERPRIVATE_H
#define LPOINTERPRIVATE_H

#include <protocols/Wayland/private/RPointerPrivate.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LDataDevicePrivate.h>
#include <LCompositor.h>
#include <LPointer.h>
#include <LSeat.h>
#include <LDNDManager.h>
#include <LClient.h>

using namespace Louvre;
using namespace Louvre::Protocols;

struct LPointer::Params
{
    /* Add here any required constructor param */
};

LPRIVATE_CLASS(LPointer)
    // Events
    void sendLeaveEvent(LSurface *surface);

    // Wayland
    LSurface *pointerFocusSurface = nullptr;
    LSurface *draggingSurface = nullptr;
    LToplevelRole *movingToplevel = nullptr;
    LToplevelRole *resizingToplevel = nullptr;

    // Toplevel Moving
    LPoint movingToplevelInitPos;
    LPoint movingToplevelInitPointerPos;
    LRect movingToplevelConstraintBounds;

    Float64 axisXprev;
    Float64 axisYprev;
    Int32 discreteXprev;
    Int32 discreteYprev;

    // Cursor
    LCursorRole *lastCursorRequest = nullptr;
    bool lastCursorRequestWasHide = false;
};

#endif // LPOINTERPRIVATE_H
