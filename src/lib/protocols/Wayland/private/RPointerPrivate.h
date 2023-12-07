#ifndef RPOINTERPRIVATE_H
#define RPOINTERPRIVATE_H

#include <protocols/Wayland/RPointer.h>

using namespace Louvre::Protocols::Wayland;
using namespace Louvre::Protocols::PointerGestures;
using namespace Louvre::Protocols::RelativePointer;

LPRIVATE_CLASS(RPointer)
    static void resource_destroy(wl_resource *resource);
    static void set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *_surface, Int32 hotspot_x, Int32 hotspot_y);
#if LOUVRE_WL_SEAT_VERSION >= 5
    static void release(wl_client *client, wl_resource *resource);
#endif

    GSeat *gSeat;
    std::list<RPointer*>::iterator seatLink;

    RGestureSwipe *rGestureSwipe = nullptr;
    RGesturePinch *rGesturePinch = nullptr;
    RGestureHold *rGestureHold = nullptr;

    RRelativePointer *rRelativePointer = nullptr;

    SerialEvents serialEvents;
};

#endif // RPOINTERPRIVATE_H
