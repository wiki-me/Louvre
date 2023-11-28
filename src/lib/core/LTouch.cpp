#include <private/LTouchPrivate.h>

using namespace Louvre;

LTouch::LTouch(Params *params) :
    LPRIVATE_INIT_UNIQUE(LTouch)
{
    L_UNUSED(params);
}

LTouch::~LTouch()
{

}

const std::list<LTouchPoint *> &LTouch::touchPoints() const
{
    return imp()->touchPoints;
}

const LTouchPoint *LTouch::sendTouchDownEvent(const LTouchDownEvent &event, LSurface *surface, const LPointF &localPos)
{

}

void LTouch::sendTouchFrameEvent(const LTouchFrameEvent &event)
{

}

void LTouch::sendTouchCancelEvent(const LTouchCancelEvent &event)
{

}
