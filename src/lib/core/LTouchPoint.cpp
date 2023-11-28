#include <private/LTouchPrivate.h>
#include <LTouchDownEvent.h>
#include <LTouchPoint.h>
#include <LSeat.h>

using namespace Louvre;

bool LTouchPoint::sendTouchMoveEvent(const LTouchMoveEvent &event, const LPointF &localPos)
{

}

bool LTouchPoint::sendTouchUpEvent(const LTouchUpEvent &event)
{

}

LSurface *LTouchPoint::surface() const
{
    return m_surface;
}

LTouchPoint::LTouchPoint(const LTouchDownEvent &event, LSurface *surface) :
    m_surface(surface),
    m_id(event.id())
{
    seat()->touch()->imp()->touchPoints.push_back(this);
    m_link = std::prev(seat()->touch()->imp()->touchPoints.end());
}

LTouchPoint::~LTouchPoint()
{
    seat()->touch()->imp()->touchPoints.erase(m_link);
}
