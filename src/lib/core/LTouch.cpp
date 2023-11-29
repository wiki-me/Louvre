#include <private/LTouchPrivate.h>
#include <LTouchPoint.h>
#include <LTouchDownEvent.h>
#include <LSeat.h>
#include <LPointer.h>

using namespace Louvre;

LTouch::LTouch(Params *params) :
    LPRIVATE_INIT_UNIQUE(LTouch)
{
    L_UNUSED(params);
}

LTouch::~LTouch() {}

LSurface *LTouch::surfaceAt(const LPoint &point)
{
    return seat()->pointer()->surfaceAt(point);
}

const std::list<LTouchPoint *> &LTouch::touchPoints() const
{
    return imp()->touchPoints;
}

const LTouchPoint *LTouch::sendTouchDownEvent(const LTouchDownEvent &event, LSurface *surface, const LPointF &localPos)
{
    if (!surface)
        return nullptr;

    // Stop if the id is already used
    for (LTouchPoint *tp : touchPoints())
        if (tp->id() == event.id())
        {
            if (surface == tp->surface())
            {
                tp->m_isReleased = false;
                return tp;
            }
            return nullptr;
        }

    return new LTouchPoint(event, surface, localPos);
}

void LTouch::sendTouchFrameEvent(const LTouchFrameEvent &event)
{
    for (std::list<LTouchPoint*>::iterator it = imp()->touchPoints.begin(); it != imp()->touchPoints.end(); it++)
    {
        (*it)->sendTouchFrameEvent(event);

        if ((*it)->m_isReleased)
            it = (*it)->destroy();
    }
}

void LTouch::sendTouchCancelEvent(const LTouchCancelEvent &event)
{
    while (!touchPoints().empty())
    {
        LTouchPoint *tp = touchPoints().back();
        tp->sendTouchCancelEvent(event);
        tp->destroy();
    }
}
