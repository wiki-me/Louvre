#ifndef RTOUCH_H
#define RTOUCH_H

#include <LResource.h>
#include <LTouch.h>
#include <LTouchDownEvent.h>
#include <LTouchUpEvent.h>

class Louvre::Protocols::Wayland::RTouch : public LResource
{
public:
    RTouch(GSeat *gSeat, Int32 id);
    LCLASS_NO_COPY(RTouch)
    ~RTouch();

    struct SerialEvents
    {
        LTouchDownEvent down;
        LTouchUpEvent up;
    };

    GSeat *seatGlobal() const;
    const SerialEvents &serialEvents() const;

    // Since 1
    bool down(LInputDevice *device, UInt32 time, UInt32 serial, RSurface *rSurface, Int32 id, Float32 x, Float32 y);
    bool up(LInputDevice *device, UInt32 time, UInt32 serial, Int32 id);
    bool motion(UInt32 time, Int32 id, Float24 x, Float24 y);
    bool frame();
    bool cancel();

    // Since 6
    bool shape(Int32 id, Float24 major, Float24 minor);
    bool orientation(Int32 id, Float24 orientation);

    LPRIVATE_IMP_UNIQUE(RTouch);
};

#endif // RTOUCH_H
