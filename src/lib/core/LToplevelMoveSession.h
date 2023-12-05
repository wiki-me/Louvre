#ifndef LTOPLEVELMOVESESSION_H
#define LTOPLEVELMOVESESSION_H

#include <LObject.h>

class Louvre::LToplevelMoveSession : public LObject
{
public:
    void setMovePointPos(const LPoint &pos);
    const std::list<LToplevelMoveSession*>::iterator stop();
    LToplevelRole *toplevel() const;
    const LEvent &triggeringEvent() const;

LPRIVATE_IMP_UNIQUE(LToplevelMoveSession)
    friend class LToplevelRole;
    LToplevelMoveSession();
    ~LToplevelMoveSession();
};

#endif // LTOPLEVELMOVESESSION_H
