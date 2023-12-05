#include <private/LToplevelResizeSessionPrivate.h>
#include <private/LToplevelRolePrivate.h>
#include <private/LSeatPrivate.h>
#include <LSurface.h>
#include <LEvent.h>

using namespace Louvre;

LToplevelResizeSession::LToplevelResizeSession() :
    LPRIVATE_INIT_UNIQUE(LToplevelResizeSession)
{
    imp()->session = this;
}

LToplevelResizeSession::~LToplevelResizeSession()
{
    delete imp()->triggeringEvent;
}

void LToplevelResizeSession::setResizePointPos(const LPoint &resizePoint)
{
    if (imp()->stopped)
        return;

    imp()->currentResizePointPos = resizePoint;
    LSize newSize = toplevel()->calculateResizeSize(imp()->initResizePointPos - resizePoint,
                                                    imp()->initToplevelSize,
                                                    imp()->edge);
    LPoint pos = toplevel()->surface()->pos();
    LSize size = toplevel()->windowGeometry().size();

    // Top
    if (imp()->bounds.y1 != LToplevelRole::EdgeDisabled && (imp()->edge ==  LToplevelRole::Top || imp()->edge ==  LToplevelRole::TopLeft || imp()->edge ==  LToplevelRole::TopRight))
    {
        if (pos.y() - (newSize.y() - size.y()) < imp()->bounds.y1)
            newSize.setH(pos.y() + size.h() - imp()->bounds.y1);
    }
    // Bottom
    else if (imp()->bounds.y2 != LToplevelRole::EdgeDisabled && (imp()->edge ==  LToplevelRole::Bottom || imp()->edge ==  LToplevelRole::BottomLeft || imp()->edge ==  LToplevelRole::BottomRight))
    {
        if (pos.y() + newSize.h() > imp()->bounds.y2)
            newSize.setH(imp()->bounds.y2 - pos.y());
    }

    // Left
    if (imp()->bounds.x1 != LToplevelRole::EdgeDisabled && (imp()->edge ==  LToplevelRole::Left || imp()->edge ==  LToplevelRole::TopLeft || imp()->edge ==  LToplevelRole::BottomLeft))
    {
        if (pos.x() - (newSize.x() - size.x()) < imp()->bounds.x1)
            newSize.setW(pos.x() + size.w() - imp()->bounds.x1);
    }
    // Right
    else if (imp()->bounds.x2 != LToplevelRole::EdgeDisabled && (imp()->edge ==  LToplevelRole::Right || imp()->edge ==  LToplevelRole::TopRight || imp()->edge ==  LToplevelRole::BottomRight))
    {
        if (pos.x() + newSize.w() > imp()->bounds.x2)
            newSize.setW(imp()->bounds.x2 - pos.x());
    }

    if (newSize.w() < imp()->toplevelMinSize.w())
        newSize.setW(imp()->toplevelMinSize.w());

    if (newSize.h() < imp()->toplevelMinSize.h())
        newSize.setH(imp()->toplevelMinSize.h());

    toplevel()->configure(newSize, LToplevelRole::Activated | LToplevelRole::Resizing);
}

const std::list<LToplevelResizeSession*>::iterator LToplevelResizeSession::stop()
{
    toplevel()->configure(toplevel()->pendingState() & ~LToplevelRole::Resizing);

    if (imp()->stopped)
        return seat()->imp()->resizeSessions.begin();

    std::list<LToplevelResizeSession*>::iterator it = imp()->unlink();
    imp()->stopped = true;

    return it;
}

LToplevelRole *LToplevelResizeSession::toplevel() const
{
    return imp()->toplevel;
}

const LEvent &LToplevelResizeSession::triggeringEvent() const
{
    return *imp()->triggeringEvent;
}
