#ifndef LTOPLEVELRESIZESESSIONPRIVATE_H
#define LTOPLEVELRESIZESESSIONPRIVATE_H

#include <LToplevelResizeSession.h>

using namespace Louvre;

LPRIVATE_CLASS(LToplevelResizeSession)
    LToplevelResizeSession *session;
    LToplevelRole *toplevel;
    LPoint initToplevelPos;
    LPoint initResizePointPos;
    LPoint currentResizePointPos;
    LSize initToplevelSize;
    LSize toplevelMinSize;
    LToplevelRole::ResizeEdge edge;
    LBox bounds;
    std::list<LToplevelResizeSession*>::iterator link;
    LEvent *triggeringEvent = nullptr;
    bool stopped = false;

    void handleGeometryChange();
    std::list<LToplevelResizeSession*>::iterator destroy();
    std::list<LToplevelResizeSession*>::iterator unlink();
};

#endif // LTOPLEVELRESIZESESSIONPRIVATE_H
