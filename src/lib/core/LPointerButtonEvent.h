#ifndef LPOINTERBUTTONEVENT_H
#define LPOINTERBUTTONEVENT_H

#include <LInputEvent.h>
#include <LPointer.h>

class Louvre::LPointerButtonEvent : public LInputEvent
{
public:
    LPointerButtonEvent();
    ~LPointerButtonEvent();

    inline void setButton(LPointer::Button button)
    {
        m_button = button;
    }

    inline LPointer::Button button() const
    {
        return m_button;
    }

    inline void setState(LPointer::ButtonState state)
    {
        m_state = state;
    }

    inline LPointer::ButtonState state() const
    {
        return m_state;
    }
protected:
    LPointer::Button m_button;
    LPointer::ButtonState m_state;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LPOINTERBUTTONEVENT_H
