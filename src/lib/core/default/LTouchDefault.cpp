#include <private/LTouchPrivate.h>
#include <LInputDevice.h>
#include <LTouchDownEvent.h>
#include <LTouchMoveEvent.h>
#include <LTouchUpEvent.h>
#include <LTouchFrameEvent.h>
#include <LTouchCancelEvent.h>
#include <LLog.h>

void LTouch::touchDownEvent(const LTouchDownEvent &event)
{
    LLog::log("TOUCH DOWN: device(%s), id(%d), x(%f), y(%f), time(%d)",
              event.device()->name(),
              event.id(),
              event.pos().x(),
              event.pos().y(),
              event.time());
}

void LTouch::touchMoveEvent(const LTouchMoveEvent &event)
{
    LLog::log("TOUCH MOVE: device(%s), id(%d), x(%f), y(%f), time(%d)",
              event.device()->name(),
              event.id(),
              event.pos().x(),
              event.pos().y(),
              event.time());
}

void LTouch::touchUpEvent(const LTouchUpEvent &event)
{
    LLog::log("TOUCH UP: device(%s), id(%d), time(%d)",
              event.device()->name(),
              event.id(),
              event.time());
}

void LTouch::touchFrameEvent(const LTouchFrameEvent &event)
{
    LLog::log("TOUCH FRAME: device(%s), time(%d)",
              event.device()->name(),
              event.time());
}

void LTouch::touchCancelEvent(const LTouchCancelEvent &event)
{
    LLog::log("TOUCH CANCEL: device(%s), time(%d)",
              event.device()->name(),
              event.time());
}
