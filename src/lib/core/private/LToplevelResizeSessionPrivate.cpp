#include <private/LToplevelResizeSessionPrivate.h>
#include <private/LToplevelRolePrivate.h>
#include <private/LSeatPrivate.h>
#include <LSurface.h>

void LToplevelResizeSession::LToplevelResizeSessionPrivate::handleGeometryChange()
{
    if (!toplevel->resizing() && stopped)
    {
        destroy();
        return;
    }

    if (!toplevel->resizing())
        return;

    if (edge ==  LToplevelRole::Top || edge ==  LToplevelRole::TopLeft || edge ==  LToplevelRole::TopRight)
        toplevel->surface()->setY(initToplevelPos.y() + (initToplevelSize.h() - toplevel->windowGeometry().h()));

    if (edge ==  LToplevelRole::Left || edge ==  LToplevelRole::TopLeft || edge ==  LToplevelRole::BottomLeft)
        toplevel->surface()->setX(initToplevelPos.x() + (initToplevelSize.w() - toplevel->windowGeometry().w()));

    if (stopped)
        toplevel->configure(toplevel->pendingState() & ~LToplevelRole::Resizing);
}

std::list<LToplevelResizeSession*>::iterator LToplevelResizeSession::LToplevelResizeSessionPrivate::destroy()
{
    std::list<LToplevelResizeSession*>::iterator it = unlink();
    toplevel->imp()->resizeSession = nullptr;
    delete session;
    return it;
}

std::list<LToplevelResizeSession*>::iterator LToplevelResizeSession::LToplevelResizeSessionPrivate::unlink()
{
    if (stopped)
        return seat()->imp()->resizeSessions.begin();

    return seat()->imp()->resizeSessions.erase(link);
}
