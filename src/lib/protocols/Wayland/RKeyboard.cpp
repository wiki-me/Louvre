#include <LTime.h>
#include <LSeat.h>
#include <private/LKeyboardPrivate.h>
#include <LClient.h>
#include <protocols/Wayland/private/GSeatPrivate.h>
#include <protocols/Wayland/private/RKeyboardPrivate.h>

#include <LCompositor.h>

using namespace Louvre;

static struct wl_keyboard_interface keyboard_implementation =
{
#if LOUVRE_WL_SEAT_VERSION >= 3
    .release = &RKeyboard::RKeyboardPrivate::release
#endif
};

RKeyboard::RKeyboard
(
    GSeat *gSeat,
    Int32 id
)
    :LResource
    (
        gSeat->client(),
        &wl_keyboard_interface,
        gSeat->version(),
        id,
        &keyboard_implementation,
        &RKeyboard::RKeyboardPrivate::resource_destroy
    ),
    LPRIVATE_INIT_UNIQUE(RKeyboard)
{
    imp()->gSeat = gSeat;
    LKeyboard *lKeyboard = seat()->keyboard();
    repeatInfo(lKeyboard->repeatRate(), lKeyboard->repeatDelay());
    keymap(lKeyboard->keymapFormat(), lKeyboard->keymapFd(), lKeyboard->keymapSize());
    gSeat->imp()->rKeyboard = this;
}

RKeyboard::~RKeyboard()
{
    if (seat()->keyboard()->grabbingKeyboardResource() == this)
        seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);

    if (seatGlobal())
        seatGlobal()->imp()->rKeyboard = nullptr;
}

GSeat *RKeyboard::seatGlobal() const
{
    return imp()->gSeat;
}

const RKeyboard::SerialEvents &RKeyboard::serialEvents() const
{
    return imp()->serialEvents;
}

bool RKeyboard::keymap(UInt32 format, Int32 fd, UInt32 size)
{
    wl_keyboard_send_keymap(resource(), format, fd, size);
    return true;
}

bool RKeyboard::enter(LInputDevice *device, UInt32 time, UInt32 serial, RSurface *rSurface, wl_array *keys)
{
    imp()->serialEvents.enter.setDevice(device);
    imp()->serialEvents.enter.setTime(time);
    imp()->serialEvents.enter.setSerial(serial);
    wl_keyboard_send_enter(resource(), serial, rSurface->resource(), keys);
    return true;
}

bool RKeyboard::leave(LInputDevice *device, UInt32 time, UInt32 serial, RSurface *rSurface)
{
    imp()->serialEvents.leave.setDevice(device);
    imp()->serialEvents.leave.setTime(time);
    imp()->serialEvents.leave.setSerial(serial);
    wl_keyboard_send_leave(resource(), serial, rSurface->resource());
    return true;
}

bool RKeyboard::key(LInputDevice *device, UInt32 time, UInt32 serial, UInt32 key, UInt32 state)
{
    imp()->serialEvents.key.setDevice(device);
    imp()->serialEvents.key.setTime(time);
    imp()->serialEvents.key.setSerial(serial);
    imp()->serialEvents.key.setKeyCode(key);
    imp()->serialEvents.key.setState((LKeyboardKeyEvent::State)state);
    wl_keyboard_send_key(resource(), serial, time, key, state);
    return true;
}

bool RKeyboard::modifiers(LInputDevice *device, UInt32 time, UInt32 serial, UInt32 modsDepressed, UInt32 modsLatched, UInt32 modsLocked, UInt32 group)
{
    imp()->serialEvents.modifiers.setDevice(device);
    imp()->serialEvents.modifiers.setTime(time);
    imp()->serialEvents.modifiers.setSerial(serial);
    imp()->serialEvents.modifiers.setModifiers({modsDepressed, modsLatched, modsLocked, group});
    wl_keyboard_send_modifiers(resource(), serial, modsDepressed, modsLatched, modsLocked, group);
    return true;
}

bool RKeyboard::repeatInfo(Int32 rate, Int32 delay)
{
#if LOUVRE_WL_SEAT_VERSION >= 4
    if (version() >= 4)
    {
        wl_keyboard_send_repeat_info(resource(), rate, delay);
        return true;
    }
#endif
    L_UNUSED(rate);
    L_UNUSED(delay);
    return false;
}
