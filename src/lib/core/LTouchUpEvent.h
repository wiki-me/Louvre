#ifndef LTOUCHUPEVENT_H
#define LTOUCHUPEVENT_H

#include <LInputEvent.h>

class Louvre::LTouchUpEvent : public LInputEvent
{
public:
    LTouchUpEvent();
    ~LTouchUpEvent();

    inline void setId(UInt32 id)
    {
        m_id = id;
    }

    inline UInt32 id() const
    {
        return m_id;
    }

protected:
    UInt32 m_id;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHUPEVENT_H
