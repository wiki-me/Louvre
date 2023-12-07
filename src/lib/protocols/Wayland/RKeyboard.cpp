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
    gSeat->imp()->keyboardResources.push_back(this);
    imp()->seatLink = std::prev(gSeat->imp()->keyboardResources.end());
    LKeyboard *lKeyboard = seat()->keyboard();
    repeatInfo(lKeyboard->repeatRate(), lKeyboard->repeatDelay());
    keymap(lKeyboard->keymapFormat(), lKeyboard->keymapFd(), lKeyboard->keymapSize());
}

RKeyboard::~RKeyboard()
{
    if (seat()->keyboard()->grabbingKeyboardResource() == this)
        seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);

    if (seatGlobal())
        seatGlobal()->imp()->keyboardResources.erase(imp()->seatLink);
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

bool RKeyboard::enter(const LKeyboardEnterEvent &event, RSurface *rSurface, wl_array *keys)
{
    imp()->serialEvents.enter = event;
    wl_keyboard_send_enter(resource(), event.serial(), rSurface->resource(), keys);
    return true;
}

bool RKeyboard::leave(const LKeyboardLeaveEvent &event, RSurface *rSurface)
{
    imp()->serialEvents.leave = event;
    wl_keyboard_send_leave(resource(), event.serial(), rSurface->resource());
    return true;
}

bool RKeyboard::key(const LKeyboardKeyEvent &event)
{
    imp()->serialEvents.key = event;
    wl_keyboard_send_key(resource(), event.serial(), event.ms(), event.keyCode(), event.state());
    return true;
}

bool RKeyboard::modifiers(const LKeyboardModifiersEvent &event)
{
    imp()->serialEvents.modifiers = event;
    wl_keyboard_send_modifiers(resource(),
                               event.serial(),
                               event.modifiers().depressed,
                               event.modifiers().latched,
                               event.modifiers().locked,
                               event.modifiers().group);
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
