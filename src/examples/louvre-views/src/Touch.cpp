#include <LTouchDownEvent.h>
#include <LTouchMoveEvent.h>
#include <LScene.h>
#include <LCursor.h>
#include "Touch.h"
#include "Global.h"

Touch::Touch(Params *params) : LTouch(params) {}

void Touch::touchDownEvent(const LTouchDownEvent &event)
{
    G::scene()->handleTouchDownEvent(event, toGlobal(cursor()->output(), event.pos()));
}

void Touch::touchMoveEvent(const LTouchMoveEvent &event)
{
    G::scene()->handleTouchMoveEvent(event, toGlobal(cursor()->output(), event.pos()));
}

void Touch::touchUpEvent(const LTouchUpEvent &event)
{
    G::scene()->handleTouchUpEvent(event);
}

void Touch::touchFrameEvent(const LTouchFrameEvent &event)
{
    G::scene()->handleTouchFrameEvent(event);
}

void Touch::touchCancelEvent(const LTouchCancelEvent &event)
{
    G::scene()->handleTouchCancelEvent(event);
}
