#include <protocols/Wayland/private/RPointerPrivate.h>
#include <protocols/PointerGestures/RGestureSwipe.h>
#include <protocols/PointerGestures/RGesturePinch.h>
#include <protocols/PointerGestures/RGestureHold.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LDataDevicePrivate.h>
#include <private/LClientPrivate.h>
#include <private/LPointerPrivate.h>
#include <private/LToplevelRolePrivate.h>
#include <private/LSeatPrivate.h>
#include <private/LCompositorPrivate.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LTime.h>
#include <LKeyboard.h>
#include <LDNDManager.h>
#include <LPointerButtonEvent.h>
#include <LPointerScrollEvent.h>
#include <LPointerMoveEvent.h>

using namespace Louvre;
using namespace Louvre::Protocols;

LPointer::LPointer(Params *params) : LPRIVATE_INIT_UNIQUE(LPointer)
{
    L_UNUSED(params);
    seat()->imp()->pointer = this;
}

LPointer::~LPointer() {}

LCursorRole *LPointer::lastCursorRequest() const
{
    return imp()->lastCursorRequest;
}

bool LPointer::lastCursorRequestWasHide() const
{
    return imp()->lastCursorRequestWasHide;
}

void LPointer::setFocus(LSurface *surface)
{
    if (surface)
        setFocus(surface, cursor()->pos() - surface->rolePos());
    else
        setFocus(nullptr, 0);
}

void LPointer::setFocus(LSurface *surface, const LPointF &localPos)
{
    if (surface)
    {
        if (focus() == surface)
            return;

        if (imp()->pendingSwipeEnd)
            sendSwipeEndEvent(LPointerSwipeEndEvent());

        if (imp()->pendingPinchEnd)
            sendPinchEndEvent(LPointerPinchEndEvent());

        if (imp()->pendingHoldEnd)
            sendHoldEndEvent(LPointerHoldEndEvent());

        imp()->sendLeaveEvent(focus());

        LPointerEnterEvent enterEvent;
        enterEvent.localPos = localPos;
        imp()->pointerFocusSurface = nullptr;

        for (Wayland::GSeat *s : surface->client()->seatGlobals())
        {
            if (s->pointerResource())
            {
                imp()->pointerFocusSurface = surface;
                s->pointerResource()->enter(enterEvent,
                                            surface->surfaceResource());
                s->pointerResource()->frame();
            }
        }
    }
    else
    {
        // Remove focus from focused surface
        if (imp()->pendingSwipeEnd)
            sendSwipeEndEvent(LPointerSwipeEndEvent());

        if (imp()->pendingPinchEnd)
            sendPinchEndEvent(LPointerPinchEndEvent());

        if (imp()->pendingHoldEnd)
            sendHoldEndEvent(LPointerHoldEndEvent());

        imp()->sendLeaveEvent(focus());
        imp()->pointerFocusSurface = nullptr;
    }
}

void LPointer::sendMoveEvent(const LPointerMoveEvent &event)
{
    if (!focus())
        return;

    for (Wayland::GSeat *s : seat()->pointer()->focus()->client()->seatGlobals())
    {
        if (s->pointerResource())
        {
            s->pointerResource()->motion(event);
            s->pointerResource()->frame();
        }
    }
}

void LPointer::sendButtonEvent(const LPointerButtonEvent &event)
{
    if (!focus())
        return;

    for (Wayland::GSeat *s : seat()->pointer()->focus()->client()->seatGlobals())
    {
        if (s->pointerResource())
        {
            s->pointerResource()->button(event);
            s->pointerResource()->frame();
        }
    }
}

void LPointer::setDraggingSurface(LSurface *surface)
{
    imp()->draggingSurface = surface;
}

void LPointer::dismissPopups()
{
    std::list<LSurface*>::const_reverse_iterator s = compositor()->surfaces().rbegin();
    for (; s!= compositor()->surfaces().rend(); s++)
    {
        if ((*s)->popup())
            (*s)->popup()->dismiss();
    }
}

LSurface *LPointer::draggingSurface() const
{
    return imp()->draggingSurface;
}

void LPointer::sendScrollEvent(const LPointerScrollEvent &event)
{
    // If no surface has focus
    if (!focus())
        return;

    Float24 aX = wl_fixed_from_double(event.axes().x());
    Float24 aY = wl_fixed_from_double(event.axes().y());
    Float24 dX = wl_fixed_from_double(event.axes120().x());
    Float24 dY = wl_fixed_from_double(event.axes120().y());

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
    {
        if (s->pointerResource())
        {
            // Since 5
            if (s->pointerResource()->axisSource(event.source()))
            {
                if (s->pointerResource()->axisRelativeDirection(WL_POINTER_AXIS_HORIZONTAL_SCROLL, 0 /* 0 = IDENTICAL */))
                    s->pointerResource()->axisRelativeDirection(WL_POINTER_AXIS_VERTICAL_SCROLL, 0 /* 0 = IDENTICAL */);

                if (event.source() == LPointerScrollEvent::Wheel)
                {
                    if (!s->pointerResource()->axisValue120(WL_POINTER_AXIS_HORIZONTAL_SCROLL, dX))
                    {
                        s->pointerResource()->axisDiscrete(WL_POINTER_AXIS_HORIZONTAL_SCROLL, aX);
                        s->pointerResource()->axisDiscrete(WL_POINTER_AXIS_VERTICAL_SCROLL, aY);
                    }
                    else
                        s->pointerResource()->axisValue120(WL_POINTER_AXIS_VERTICAL_SCROLL, dY);
                }

                if (event.axes().x() == 0.0 && imp()->axisXprev != 0.0)
                    s->pointerResource()->axisStop(event.ms(), WL_POINTER_AXIS_HORIZONTAL_SCROLL);
                else
                    s->pointerResource()->axis(event.ms(), WL_POINTER_AXIS_HORIZONTAL_SCROLL, aX);

                if (event.axes().y() == 0.0 && imp()->axisYprev != 0.0)
                   s->pointerResource()->axisStop(event.ms(), WL_POINTER_AXIS_VERTICAL_SCROLL);
                else
                    s->pointerResource()->axis(event.ms(), WL_POINTER_AXIS_VERTICAL_SCROLL, aY);

                s->pointerResource()->frame();
            }
            // Since 1
            else
            {
                s->pointerResource()->axis(
                    event.ms(),
                    aX,
                    aY);
            }
        }
    }

    imp()->axisXprev = event.axes().x();
    imp()->axisYprev = event.axes().y();
}

void LPointer::sendSwipeBeginEvent(const LPointerSwipeBeginEvent &event)
{
    if (!focus() || imp()->pendingSwipeEnd)
        return;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
    {
        if (s->pointerResource() && s->pointerResource()->gestureSwipeResource())
        {
            imp()->pendingSwipeEnd = true;
            s->pointerResource()->gestureSwipeResource()->begin(event, focus()->surfaceResource());
        }
    }
}

void LPointer::sendSwipeUpdateEvent(const LPointerSwipeUpdateEvent &event)
{
    if (!focus() || !imp()->pendingSwipeEnd)
        return;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
        if (s->pointerResource() && s->pointerResource()->gestureSwipeResource())
            s->pointerResource()->gestureSwipeResource()->update(event);
}

void LPointer::sendSwipeEndEvent(const LPointerSwipeEndEvent &event)
{
    if (!focus() || !imp()->pendingSwipeEnd)
        return;

    imp()->pendingSwipeEnd = false;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
        if (s->pointerResource() && s->pointerResource()->gestureSwipeResource())
            s->pointerResource()->gestureSwipeResource()->end(event);
}

void LPointer::sendPinchBeginEvent(const LPointerPinchBeginEvent &event)
{
    if (!focus() || imp()->pendingPinchEnd)
        return;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
    {
        if (s->pointerResource() && s->pointerResource()->gesturePinchResource())
        {
            imp()->pendingPinchEnd = true;
            s->pointerResource()->gesturePinchResource()->begin(event, focus()->surfaceResource());
        }
    }
}

void LPointer::sendPinchUpdateEvent(const LPointerPinchUpdateEvent &event)
{
    if (!focus() || !imp()->pendingPinchEnd)
        return;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
        if (s->pointerResource() && s->pointerResource()->gesturePinchResource())
            s->pointerResource()->gesturePinchResource()->update(event);
}

void LPointer::sendPinchEndEvent(const LPointerPinchEndEvent &event)
{
    if (!focus() || !imp()->pendingPinchEnd)
        return;

    imp()->pendingPinchEnd = false;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
        if (s->pointerResource() && s->pointerResource()->gesturePinchResource())
            s->pointerResource()->gesturePinchResource()->end(event);
}

void LPointer::sendHoldBeginEvent(const LPointerHoldBeginEvent &event)
{
    if (!focus() || imp()->pendingHoldEnd)
        return;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
    {
        if (s->pointerResource() && s->pointerResource()->gestureHoldResource())
        {
            imp()->pendingHoldEnd = true;
            s->pointerResource()->gestureHoldResource()->begin(event, focus()->surfaceResource());
        }
    }
}

void LPointer::sendHoldEndEvent(const LPointerHoldEndEvent &event)
{
    if (!focus() || !imp()->pendingHoldEnd)
        return;

    imp()->pendingHoldEnd = false;

    for (Wayland::GSeat *s : focus()->client()->seatGlobals())
        if (s->pointerResource() && s->pointerResource()->gestureHoldResource())
            s->pointerResource()->gestureHoldResource()->end(event);
}

LSurface *LPointer::surfaceAt(const LPoint &point)
{
    retry:
    compositor()->imp()->surfacesListChanged = false;

    for (std::list<LSurface*>::const_reverse_iterator s = compositor()->surfaces().rbegin(); s != compositor()->surfaces().rend(); s++)
        if ((*s)->mapped() && !(*s)->minimized())
        {
            if ((*s)->inputRegion().containsPoint(point - (*s)->rolePos()))
                return *s;

            if (compositor()->imp()->surfacesListChanged)
                goto retry;
        }

    return nullptr;
}

LSurface *LPointer::focus() const
{
    return imp()->pointerFocusSurface;
}

void LPointer::LPointerPrivate::sendLeaveEvent(LSurface *surface)
{
    if (!surface)
        return;

    LPointerLeaveEvent leaveEvent;

    for (Wayland::GSeat *s : surface->client()->seatGlobals())
    {
        if (s->pointerResource())
        {  
            s->pointerResource()->leave(leaveEvent, surface->surfaceResource());
            s->pointerResource()->frame();
        }
    }
}
