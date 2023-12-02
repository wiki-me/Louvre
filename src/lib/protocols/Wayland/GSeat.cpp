#include "LLog.h"
#include <protocols/Wayland/private/RKeyboardPrivate.h>
#include <protocols/Wayland/private/RPointerPrivate.h>
#include <protocols/Wayland/private/RTouchPrivate.h>
#include <protocols/Wayland/private/RDataDevicePrivate.h>
#include <protocols/Wayland/private/GSeatPrivate.h>
#include <private/LClientPrivate.h>
#include <LCompositor.h>
#include <LTouchPoint.h>

GSeat::GSeat
(
    wl_client *client,
    const wl_interface *interface,
    Int32 version,
    UInt32 id,
    const void *implementation,
    wl_resource_destroy_func_t destroy
)
    :LResource
    (
        client,
        interface,
        version,
        id,
        implementation,
        destroy
    ),
    LPRIVATE_INIT_UNIQUE(GSeat)
{
    this->client()->imp()->seatGlobals.push_back(this);
    imp()->clientLink = std::prev(this->client()->imp()->seatGlobals.end());
    capabilities(seat()->inputCapabilities());
    name(seat()->name());
}

GSeat::~GSeat()
{
    client()->imp()->seatGlobals.erase(imp()->clientLink);

    if (keyboardResource())
        keyboardResource()->imp()->gSeat = nullptr;

    if (pointerResource())
        pointerResource()->imp()->gSeat = nullptr;

    if (touchResource())
        touchResource()->imp()->gSeat = nullptr;

    if (dataDeviceResource())
        dataDeviceResource()->imp()->gSeat = nullptr;
}

LEvent *GSeat::findSerialEventMatch(UInt32 serial) const
{
    if (pointerResource())
    {
        if (pointerResource()->imp()->serialEvents.enter.serial() == serial)
        {
            return new LPointerEnterEvent(pointerResource()->imp()->serialEvents.enter);
        }
        else if (pointerResource()->imp()->serialEvents.leave.serial() == serial)
        {
            return new LPointerLeaveEvent(pointerResource()->imp()->serialEvents.leave);
        }
        else if (pointerResource()->imp()->serialEvents.button.serial() == serial)
        {
            return new LPointerButtonEvent(pointerResource()->imp()->serialEvents.button);
        }
    }

    if (keyboardResource())
    {
        if (keyboardResource()->imp()->serialEvents.enter.serial() == serial)
        {
            return new LKeyboardEnterEvent(keyboardResource()->imp()->serialEvents.enter);
        }
        else if (keyboardResource()->imp()->serialEvents.leave.serial() == serial)
        {
            return new LKeyboardLeaveEvent(keyboardResource()->imp()->serialEvents.leave);
        }
        else if (keyboardResource()->imp()->serialEvents.key.serial() == serial)
        {
            return new LKeyboardKeyEvent(keyboardResource()->imp()->serialEvents.key);
        }
        else if (keyboardResource()->imp()->serialEvents.modifiers.serial() == serial)
        {
            return new LKeyboardModifiersEvent(keyboardResource()->imp()->serialEvents.modifiers);
        }
    }

    if (touchResource())
    {
        if (touchResource()->imp()->serialEvents.down.serial() == serial)
        {
            return new LTouchDownEvent(touchResource()->imp()->serialEvents.down);
        }
        else if (touchResource()->imp()->serialEvents.up.serial() == serial)
        {
            return new LTouchUpEvent(touchResource()->imp()->serialEvents.up);
        }
    }

    return nullptr;
}

RKeyboard *GSeat::keyboardResource() const
{
    return imp()->rKeyboard;
}

RPointer *GSeat::pointerResource() const
{
    return imp()->rPointer;
}

RTouch *GSeat::touchResource() const
{
    return imp()->rTouch;
}

RDataDevice *GSeat::dataDeviceResource() const
{
    return imp()->rDataDevice;
}

bool GSeat::capabilities(UInt32 capabilities)
{
    wl_seat_send_capabilities(resource(), capabilities);
    return true;
}

bool GSeat::name(const char *name)
{
#if LOUVRE_WL_SEAT_VERSION >= 2
    if (version() >= 2)
    {
        wl_seat_send_name(resource(), name);
        return true;
    }
#endif
    L_UNUSED(name);
    return false;
}
