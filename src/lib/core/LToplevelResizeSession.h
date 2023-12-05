#ifndef LTOPLEVELRESIZESESSION_H
#define LTOPLEVELRESIZESESSION_H

#include <LToplevelRole.h>

class Louvre::LToplevelResizeSession : public LObject
{
public:
    void setResizePointPos(const LPoint &pos);
    const std::list<LToplevelResizeSession*>::iterator stop();
    LToplevelRole *toplevel() const;
    const LEvent *triggeringEvent() const;

    LPRIVATE_IMP_UNIQUE(LToplevelResizeSession)
    friend class LToplevelRole;
    LToplevelResizeSession();
    ~LToplevelResizeSession();
};

#endif // LTOPLEVELRESIZESESSION_H
