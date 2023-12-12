#ifndef LSCENETOUCHPOINTPRIVATE_H
#define LSCENETOUCHPOINTPRIVATE_H

#include <LSceneTouchPoint.h>

using namespace Louvre;

LPRIVATE_CLASS(LSceneTouchPoint)
    Int32 id = 0;
    LPointF pos;
    bool isPressed = true;
    bool listChanged = false;
    std::list<LView*> views;
    LScene *scene = nullptr;
    std::list<LSceneTouchPoint*>::iterator sceneLink;
};

#endif // LSCENETOUCHPOINTPRIVATE_H
