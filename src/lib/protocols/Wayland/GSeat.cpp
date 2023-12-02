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
            return pointerResource()->imp()->serialEvents.enter.copy();
        else if (pointerResource()->imp()->serialEvents.leave.serial() == serial)
            return pointerResource()->imp()->serialEvents.leave.copy();
        else if (pointerResource()->imp()->serialEvents.button.serial() == serial)
            return pointerResource()->imp()->serialEvents.button.copy();
    }

    if (keyboardResource())
    {
        if (keyboardResource()->imp()->serialEvents.enter.serial() == serial)
            return keyboardResource()->imp()->serialEvents.enter.copy();
        else if (keyboardResource()->imp()->serialEvents.leave.serial() == serial)
            return keyboardResource()->imp()->serialEvents.leave.copy();
        else if (keyboardResource()->imp()->serialEvents.key.serial() == serial)
            return keyboardResource()->imp()->serialEvents.key.copy();
        else if (keyboardResource()->imp()->serialEvents.modifiers.serial() == serial)
            return keyboardResource()->imp()->serialEvents.modifiers.copy();
    }

    if (touchResource())
    {
        if (touchResource()->imp()->serialEvents.down.serial() == serial)
            return touchResource()->imp()->serialEvents.down.copy();
        else if (touchResource()->imp()->serialEvents.up.serial() == serial)
            return touchResource()->imp()->serialEvents.up.copy();
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
