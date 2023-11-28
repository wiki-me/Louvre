#ifndef RTOUCH_H
#define RTOUCH_H

#include <LResource.h>
#include <LTouch.h>

class Louvre::Protocols::Wayland::RTouch : public LResource
{
public:
    RTouch(GSeat *gSeat, Int32 id);
    LCLASS_NO_COPY(RTouch)
    ~RTouch();

    struct LastEventSerials
    {
        UInt32 down = 0;
        UInt32 up = 0;
    };

    GSeat *seatGlobal() const;
    const LastEventSerials &serials() const;

    // Since 1
    bool down(UInt32 serial, UInt32 time, RSurface *rSurface, Int32 id, Float24 x, Float24 y);
    bool up(UInt32 serial, UInt32 time, Int32 id);
    bool motion(UInt32 time, Int32 id, Float24 x, Float24 y);
    bool frame();
    bool cancel();

    // Since 6
    bool shape(Int32 id, Float24 major, Float24 minor);
    bool orientation(Int32 id, Float24 orientation);

    LPRIVATE_IMP_UNIQUE(RTouch);
};

#endif // RTOUCH_H
