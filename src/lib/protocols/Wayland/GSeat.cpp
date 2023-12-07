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

    for (auto res : keyboardResources())
        res->imp()->gSeat = nullptr;

    for (auto res : pointerResources())
        res->imp()->gSeat = nullptr;

    for (auto res : touchResources())
        res->imp()->gSeat = nullptr;

    if (dataDeviceResource())
        dataDeviceResource()->imp()->gSeat = nullptr;
}

LEvent *GSeat::findSerialEventMatch(UInt32 serial) const
{
    for (auto res : pointerResources())
    {
        if (res->imp()->serialEvents.enter.serial() == serial)
            return &res->imp()->serialEvents.enter;
        else if (res->imp()->serialEvents.leave.serial() == serial)
            return &res->imp()->serialEvents.leave;
        else if (res->imp()->serialEvents.button.serial() == serial)
            return &res->imp()->serialEvents.button;
    }

    for (auto res : keyboardResources())
    {
        if (res->imp()->serialEvents.enter.serial() == serial)
            return &res->imp()->serialEvents.enter;
        else if (res->imp()->serialEvents.leave.serial() == serial)
            return &res->imp()->serialEvents.leave;
        else if (res->imp()->serialEvents.key.serial() == serial)
            return &res->imp()->serialEvents.key;
        else if (res->imp()->serialEvents.modifiers.serial() == serial)
            return &res->imp()->serialEvents.modifiers;
    }

    for (auto res : touchResources())
    {
        if (res->imp()->serialEvents.down.serial() == serial)
            return &res->imp()->serialEvents.down;
        else if (res->imp()->serialEvents.up.serial() == serial)
            return &res->imp()->serialEvents.up;
    }

    return nullptr;
}

const std::list<RKeyboard *> &GSeat::keyboardResources() const
{
    return imp()->keyboardResources;
}

const std::list<RPointer *> &GSeat::pointerResources() const
{
    return imp()->pointerResources;
}

const std::list<RTouch *> &GSeat::touchResources() const
{
    return imp()->touchResources;
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
