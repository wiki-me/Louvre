#include <private/LTouchPointPrivate.h>
#include <private/LTouchPrivate.h>
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

LTouchPoint *LTouch::createTouchPoint(const LTouchDownEvent &event)
{
    for (LTouchPoint *tp : touchPoints())
        if (tp->id() == event.id())
            return tp;

    return new LTouchPoint(event);
}

LTouchPoint *LTouch::findTouchPoint(Int32 id) const
{
    for (LTouchPoint *tp : touchPoints())
        if (tp->id() == id)
            return tp;

    return nullptr;
}

void LTouch::sendFrameEvent(const LTouchFrameEvent &event)
{
    L_UNUSED(event);

    LTouchPoint *tp;
    for (std::list<LTouchPoint*>::iterator it = imp()->touchPoints.begin(); it != imp()->touchPoints.end(); it++)
    {
        tp = *it;
        tp->imp()->sendTouchFrameEvent();

        if (!tp->isPressed())
        {
            it = imp()->touchPoints.erase(tp->imp()->link);
            delete tp;
        }
    }
}

void LTouch::sendCancelEvent(const LTouchCancelEvent &event)
{
    L_UNUSED(event);

    LTouchPoint *tp;
    while (!touchPoints().empty())
    {
        tp = touchPoints().back();
        tp->imp()->sendTouchCancelEvent();
        imp()->touchPoints.pop_back();
        delete tp;
    }
}
