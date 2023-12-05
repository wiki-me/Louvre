#include <private/LToplevelMoveSessionPrivate.h>
#include <private/LToplevelRolePrivate.h>
#include <private/LSeatPrivate.h>

using namespace Louvre;

LToplevelMoveSession::LToplevelMoveSession() :
    LPRIVATE_INIT_UNIQUE(LToplevelMoveSession)
{}

LToplevelMoveSession::~LToplevelMoveSession() {}

void LToplevelMoveSession::setMovePointPos(const LPoint &pos)
{
    LPoint newPos = imp()->initToplevelPos - imp()->initMovePointPos + pos;

    if (imp()->bounds.x2 != LToplevelRole::EdgeDisabled && newPos.x() > imp()->bounds.x2)
        newPos.setX(imp()->bounds.x2);

    if (imp()->bounds.x1 != LToplevelRole::EdgeDisabled && newPos.x() < imp()->bounds.x1)
        newPos.setX(imp()->bounds.x1);

    if (imp()->bounds.y2 != LToplevelRole::EdgeDisabled && newPos.y() > imp()->bounds.y2)
        newPos.setY(imp()->bounds.y2);

    if (imp()->bounds.y1 != LToplevelRole::EdgeDisabled && newPos.y() < imp()->bounds.y1)
        newPos.setY(imp()->bounds.y1);

    toplevel()->surface()->setPos(newPos);
}

const std::list<LToplevelMoveSession*>::iterator LToplevelMoveSession::stop()
{
    delete imp()->triggeringEvent;
    std::list<LToplevelMoveSession*>::iterator it = seat()->imp()->moveSessions.erase(imp()->link);
    toplevel()->imp()->moveSession = nullptr;
    delete this;
    return it;
}

LToplevelRole *LToplevelMoveSession::toplevel() const
{
    return imp()->toplevel;
}

const LEvent &LToplevelMoveSession::triggeringEvent() const
{
    return *imp()->triggeringEvent;
}
