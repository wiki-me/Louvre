#ifndef LTOUCHUPEVENT_H
#define LTOUCHUPEVENT_H

#include <LTouchEvent.h>

class Louvre::LTouchUpEvent : public LTouchEvent
{
public:
    LTouchUpEvent();
    virtual ~LTouchUpEvent() {}
    virtual LEvent *copy() const override;

    inline void setId(Int32 id)
    {
        m_id = id;
    }

    inline Int32 id() const
    {
        return m_id;
    }

protected:
    Int32 m_id;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHUPEVENT_H
