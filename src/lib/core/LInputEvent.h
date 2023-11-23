#ifndef LINPUTEVENT_H
#define LINPUTEVENT_H

#include <LObject.h>

class Louvre::LInputEvent : public LObject
{
public:
    LInputEvent();

protected:
    friend class LInputBackend;
    void *m_backendData = nullptr;
};

#endif // LINPUTEVENT_H
