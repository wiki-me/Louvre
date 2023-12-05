#include <protocols/Wayland/private/RTouchPrivate.h>
#include <protocols/Wayland/private/GSeatPrivate.h>
#include <protocols/Wayland/RSurface.h>

static struct wl_touch_interface touch_implementation =
{
#if LOUVRE_WL_SEAT_VERSION >= 3
        .release = &RTouch::RTouchPrivate::release
#endif
};

RTouch::RTouch
    (
        GSeat *gSeat,
        Int32 id
    )
    :LResource
    (
        gSeat->client(),
        &wl_touch_interface,
        gSeat->version(),
        id,
        &touch_implementation,
        &RTouch::RTouchPrivate::resource_destroy
    ),
    LPRIVATE_INIT_UNIQUE(RTouch)
{
    imp()->gSeat = gSeat;
    gSeat->imp()->rTouch = this;
}

RTouch::~RTouch()
{
    if (seatGlobal())
        seatGlobal()->imp()->rTouch = nullptr;
}

GSeat *RTouch::seatGlobal() const
{
    return imp()->gSeat;
}

const RTouch::SerialEvents &RTouch::serialEvents() const
{
    return imp()->serialEvents;
}

bool RTouch::down(const LTouchDownEvent &event, RSurface *rSurface)
{
    imp()->serialEvents.down = event;
    wl_touch_send_down(resource(),
                       event.serial(),
                       event.ms(),
                       rSurface->resource(),
                       event.id(),
                       wl_fixed_from_double(event.localPos.x()),
                       wl_fixed_from_double(event.localPos.y()));
    return true;
}

bool RTouch::up(const LTouchUpEvent &event)
{
    imp()->serialEvents.up = event;
    wl_touch_send_up(resource(), event.serial(), event.ms(), event.id());
    return true;
}

bool RTouch::motion(UInt32 time, Int32 id, Float24 x, Float24 y)
{
    wl_touch_send_motion(resource(), time, id, x, y);
    return true;
}

bool RTouch::frame()
{
    wl_touch_send_frame(resource());
    return true;
}

bool RTouch::cancel()
{
    wl_touch_send_cancel(resource());
    return true;
}

bool RTouch::shape(Int32 id, Float24 major, Float24 minor)
{
#if LOUVRE_WL_SEAT_VERSION >= 6
    if (version() >= 6)
    {
        wl_touch_send_shape(resource(), id, major, minor);
        return true;
    }
#endif
    L_UNUSED(id);
    L_UNUSED(major);
    L_UNUSED(minor);
    return false;
}

bool RTouch::orientation(Int32 id, Float24 orientation)
{
#if LOUVRE_WL_SEAT_VERSION >= 6
    if (version() >= 6)
    {
        wl_touch_send_orientation(resource(), id, orientation);
        return true;
    }
#endif
    L_UNUSED(id);
    L_UNUSED(orientation);
    return false;
}
