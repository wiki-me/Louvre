#ifndef LKEYBOARDKEYEVENT_H
#define LKEYBOARDKEYEVENT_H

#include <LKeyboard.h>
#include <LInputEvent.h>
#include <LTime.h>

class Louvre::LKeyboardKeyEvent : public LInputEvent
{
public:
    LKeyboardKeyEvent();
    ~LKeyboardKeyEvent();

    inline void setKeyCode(UInt32 keyCode)
    {
        m_key = keyCode;
    }

    inline UInt32 keyCode() const
    {
        return m_key;
    }

    inline void setState(LKeyboard::KeyState state)
    {
        m_state = state;
    }

    inline LKeyboard::KeyState state() const
    {
        return m_state;
    }

protected:
    UInt32 m_key;
    LKeyboard::KeyState m_state;
private:
    friend class LInputBackend;
    void notify();
};
#endif // LKEYBOARDKEYEVENT_H
