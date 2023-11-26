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

    inline void sendMoveEvent(const LPointF &localPos, UInt32 time)
    {
        Float24 x = wl_fixed_from_double(localPos.x());
        Float24 y = wl_fixed_from_double(localPos.y());

        if (seat()->dndManager()->focus())
            seat()->dndManager()->focus()->client()->dataDevice().imp()->sendDNDMotionEventS(x, y);

        for (Wayland::GSeat *s : seat()->pointer()->focus()->client()->seatGlobals())
        {
            if (s->pointerResource())
            {
                s->pointerResource()->motion(time, x, y);
                s->pointerResource()->frame();
            }
        }
    }

    inline void sendButtonEvent(Button button, ButtonState state, UInt32 time)
    {
        for (Wayland::GSeat *s : seat()->pointer()->focus()->client()->seatGlobals())
        {
            if (s->pointerResource())
            {
                s->pointerResource()->imp()->serials.button = LCompositor::nextSerial();
                s->pointerResource()->button(
                    s->pointerResource()->imp()->serials.button,
                    time, button, state);
                s->pointerResource()->frame();
            }
        }
    }
};

#endif // LPOINTERPRIVATE_H
