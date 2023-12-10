#ifndef TESTVIEW_H
#define TESTVIEW_H

#include <LSolidColorView.h>
#include <LPointerSwipeUpdateEvent.h>
#include <LPointerPinchUpdateEvent.h>

using namespace Louvre;

class TestView : public LSolidColorView
{
public:
    TestView(LView *parent) : LSolidColorView(parent)
    {
        setColor(0.f, 0.f, 0.f);
        enablePointerEvents(true);
        setSize(200, 200);
    }

    void pointerSwipeBeginEvent(const LPointerSwipeBeginEvent &) override
    {
        setColor(1.f, 0.f, 0.f);
    }

    void pointerSwipeUpdateEvent(const LPointerSwipeUpdateEvent &event) override
    {
        setPos(pos() + event.delta());
    }

    void pointerSwipeEndEvent(const LPointerSwipeEndEvent &) override
    {
        setColor(0.f, 0.f, 0.f);
    }

    void pointerPinchBeginEvent(const LPointerPinchBeginEvent &) override
    {
        setColor(0.f, 1.f, 0.f);
    }

    void pointerPinchUpdateEvent(const LPointerPinchUpdateEvent &event) override
    {
        setPos(pos() + event.delta());
        setSize(LSize(200) * event.scale());
    }

    void pointerPinchEndEvent(const LPointerPinchEndEvent &) override
    {
        setColor(0.f, 0.f, 0.f);
    }

    void pointerHoldBeginEvent(const LPointerHoldBeginEvent &) override
    {
        setColor(0.f, 0.f, 1.f);
    }

    void pointerHoldEndEvent(const LPointerHoldEndEvent &) override
    {
        setColor(0.f, 0.f, 0.f);
    }
};

#endif // TESTVIEW_H
