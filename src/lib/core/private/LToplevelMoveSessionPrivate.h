#ifndef LTOPLEVELMOVESESSIONPRIVATE_H
#define LTOPLEVELMOVESESSIONPRIVATE_H

#include <LToplevelMoveSession.h>
#include <LRect.h>

using namespace Louvre;

LPRIVATE_CLASS(LToplevelMoveSession)
    LToplevelRole *toplevel = nullptr;
    LPoint initToplevelPos;
    LPoint initMovePointPos;
    LBox bounds;
    LEvent *triggeringEvent = nullptr;
    std::list<LToplevelMoveSession*>::iterator link;
};

#endif // LTOPLEVELMOVESESSIONPRIVATE_H
