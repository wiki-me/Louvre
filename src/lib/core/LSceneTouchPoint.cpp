#include <private/LSceneTouchPointPrivate.h>
#include <private/LScenePrivate.h>
#include <LTouchDownEvent.h>

using namespace Louvre;

LSceneTouchPoint::LSceneTouchPoint(LScene *scene, const LTouchDownEvent &event) :
    LPRIVATE_INIT_UNIQUE(LSceneTouchPoint)
{
    imp()->scene = scene;
    imp()->id = event.id();
    imp()->pos = event.pos();
    scene->imp()->touchPoints.push_back(this);
    imp()->sceneLink = std::prev(scene->imp()->touchPoints.end());
}

LSceneTouchPoint::~LSceneTouchPoint() {}

std::list<LSceneTouchPoint*>::iterator LSceneTouchPoint::destroy()
{
    std::list<LSceneTouchPoint*>::iterator it = imp()->scene->imp()->touchPoints.erase(imp()->sceneLink);
    imp()->scene->imp()->touchPointsListChanged = true;
    delete this;
    return it;
}

LScene *LSceneTouchPoint::scene() const
{
    return imp()->scene;
}

Int32 LSceneTouchPoint::id() const
{
    return imp()->id;
}

bool LSceneTouchPoint::isPressed() const
{
    return imp()->isPressed;
}

const std::list<LView *> &LSceneTouchPoint::views() const
{
    return imp()->views;
}

const LPointF &LSceneTouchPoint::pos() const
{
    return imp()->pos;
}

