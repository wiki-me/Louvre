#include <protocols/Wayland/private/RTouchPrivate.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LTouchPointPrivate.h>
#include <LSurface.h>
#include <LClient.h>
#include <LCompositor.h>

using namespace Louvre::Protocols::Wayland;

void LTouchPoint::LTouchPointPrivate::resetSerials()
{
    lastDownEvent.setSerial(0);
    lastUpEvent.setSerial(0);
    lastMoveEvent.setSerial(0);
}

void LTouchPoint::LTouchPointPrivate::sendTouchDownEvent(const LTouchDownEvent &event)
{
    if (!surface)
        return;

    for (GSeat *gSeat : surface->client()->seatGlobals())
        if (gSeat->touchResource())
            gSeat->touchResource()->down(event, surface->surfaceResource());
}

void LTouchPoint::LTouchPointPrivate::sendTouchFrameEvent()
{
    if (!surface)
        return;

    for (GSeat *gSeat : surface->client()->seatGlobals())
        if (gSeat->touchResource())
            gSeat->touchResource()->frame();
}

void LTouchPoint::LTouchPointPrivate::sendTouchCancelEvent()
{
    if (!surface)
        return;

    for (GSeat *gSeat : surface->client()->seatGlobals())
        if (gSeat->touchResource())
            gSeat->touchResource()->cancel();
}
