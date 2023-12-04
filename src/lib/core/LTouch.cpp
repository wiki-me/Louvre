#include <private/LTouchPointPrivate.h>
#include <private/LTouchPrivate.h>
#include <LTouchDownEvent.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LOutput.h>

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

LPointF LTouch::toGlobal(LOutput *output, const LPointF &touchPointPos)
{
    if (!output)
        return touchPointPos;

    LPointF point;

    switch (output->transform())
    {
    case LFramebuffer::Normal:
        point = output->size() * touchPointPos;
        break;
    case LFramebuffer::Flipped:
        point.setX(output->size().w() * (1.f - touchPointPos.x()));
        point.setY(output->size().h() * touchPointPos.y());
        break;
    case LFramebuffer::Clock90:
        point.setX(output->size().w() * (1.f - touchPointPos.y()));
        point.setY(output->size().h() * touchPointPos.x());
        break;
    case LFramebuffer::Clock180:
        point.setX(output->size().w() * touchPointPos.x());
        point.setY(output->size().h() * (1.f - touchPointPos.y()));
        break;
    }

    return point + output->pos();
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
