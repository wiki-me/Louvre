#include <protocols/Wayland/private/RTouchPrivate.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LTouchPrivate.h>
#include <LCompositor.h>
#include <LTouchMoveEvent.h>
#include <LTouchDownEvent.h>
#include <LTouchPoint.h>
#include <LTouchUpEvent.h>
#include <LClient.h>
#include <LSeat.h>

using namespace Louvre;
using namespace Louvre::Protocols::Wayland;

bool LTouchPoint::sendTouchMoveEvent(const LTouchMoveEvent &event, const LPointF &localPos)
{
    if (event.id() != id())
        return false;

    Float24 x = wl_fixed_from_double(localPos.x());
    Float24 y = wl_fixed_from_double(localPos.y());

    for (GSeat *gSeat : surface()->client()->seatGlobals())
    {
        if (gSeat->touchResource())
        {
            gSeat->touchResource()->motion(
                event.time(),
                id(),
                x,
                y);
        }
    }

    return true;
}

bool LTouchPoint::sendTouchUpEvent(const LTouchUpEvent &event)
{
    if (event.id() != id())
        return false;

    m_isReleased = true;

    for (GSeat *gSeat : surface()->client()->seatGlobals())
    {
        if (gSeat->touchResource())
        {
            gSeat->touchResource()->imp()->serials.up = LCompositor::nextSerial();
            gSeat->touchResource()->up(
                gSeat->touchResource()->imp()->serials.up,
                event.time(),
                id());
        }
    }

    return true;
}

LTouchPoint::LTouchPoint(const LTouchDownEvent &event, LSurface *surface, const LPointF &localPos) :
    m_surface(surface),
    m_id(event.id())
{
    seat()->touch()->imp()->touchPoints.push_back(this);
    m_link = std::prev(seat()->touch()->imp()->touchPoints.end());

    Float24 x = wl_fixed_from_double(localPos.x());
    Float24 y = wl_fixed_from_double(localPos.y());

    for (GSeat *gSeat : surface->client()->seatGlobals())
    {
        if (gSeat->touchResource())
        {
            gSeat->touchResource()->imp()->serials.down = LCompositor::nextSerial();
            gSeat->touchResource()->down(gSeat->touchResource()->imp()->serials.down,
                                         event.time(),
                                         surface->surfaceResource(),
                                         event.id(),
                                         x,
                                         y);
        }
    }
}

LTouchPoint::~LTouchPoint() {}

void LTouchPoint::sendTouchFrameEvent(const LTouchFrameEvent &)
{
    for (GSeat *gSeat : surface()->client()->seatGlobals())
        if (gSeat->touchResource())
            gSeat->touchResource()->frame();
}

void LTouchPoint::sendTouchCancelEvent(const LTouchCancelEvent &)
{
    for (GSeat *gSeat : surface()->client()->seatGlobals())
        if (gSeat->touchResource())
            gSeat->touchResource()->cancel();
}

const std::list<LTouchPoint*>::iterator LTouchPoint::destroy()
{
    std::list<LTouchPoint*>::iterator it = seat()->touch()->imp()->touchPoints.erase(m_link);
    delete this;
    return it;
}
