#ifndef LKEYBOARDKEYEVENT_H
#define LKEYBOARDKEYEVENT_H

#include <LKeyboardEvent.h>
#include <LKeyboard.h>
#include <LTime.h>

class Louvre::LKeyboardKeyEvent : public LKeyboardEvent
{
public:

    /**
     * @brief Key states.
     *
     * Enum with the possible states of a key.
     */
    enum State : UInt32
    {
        /// The key is not being pressed
        Released = 0,

        /// The key is pressed
        Pressed = 1
    };

    LKeyboardKeyEvent();
    virtual ~LKeyboardKeyEvent() {}
    virtual LEvent *copy() const override;

    inline void setKeyCode(UInt32 keyCode)
    {
        m_key = keyCode;
    }

    inline UInt32 keyCode() const
    {
        return m_key;
    }

    inline void setState(State state)
    {
        m_state = state;
    }

    inline State state() const
    {
        return m_state;
    }
protected:
    UInt32 m_key;
    State m_state;
private:
    friend class LInputBackend;
    void notify();
};
#endif // LKEYBOARDKEYEVENT_H
