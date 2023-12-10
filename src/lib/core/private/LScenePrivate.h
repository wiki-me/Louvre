#ifndef LSCENEPRIVATE_H
#define LSCENEPRIVATE_H

#include <LRegion.h>
#include <LPointerEnterEvent.h>
#include <LPointerMoveEvent.h>
#include <LPointerLeaveEvent.h>
#include <LPointerButtonEvent.h>
#include <LPointerScrollEvent.h>
#include <LPointerSwipeEndEvent.h>
#include <LPointerPinchEndEvent.h>
#include <LPointerHoldEndEvent.h>
#include <LKeyboardKeyEvent.h>
#include <LSceneView.h>
#include <LScene.h>
#include <mutex>

using namespace Louvre;

LPRIVATE_CLASS(LScene)
    std::mutex mutex;
    LSceneView view;
    bool pointerIsBlocked = false;

    bool listChanged = false;
    bool keyboardListChanged = false;
    bool pointerListChanged = false;

    // Prevent recursive calls
    bool handlingPointerMove = false;
    bool handlingPointerButton = false;
    bool handlingPointerScrollEvent = false;
    bool handlingPointerSwipeBeginEvent = false;
    bool handlingPointerSwipeUpdateEvent = false;
    bool handlingPointerSwipeEndEvent = false;
    bool handlingPointerPinchBeginEvent = false;
    bool handlingPointerPinchUpdateEvent = false;
    bool handlingPointerPinchEndEvent = false;
    bool handlingPointerHoldBeginEvent = false;
    bool handlingPointerHoldEndEvent = false;
    bool handlingKeyEvent = false;

    std::list<LView*> pointerFocus;
    std::list<LView*> keyboardFocus;
    std::list<LView*> touchFocus;

    LPointF pointerMoveEventOutLocalPos;
    LPointerEnterEvent currentPointerEnterEvent;
    LPointerMoveEvent currentPointerMoveEvent;
    LPointerLeaveEvent currentPointerLeaveEvent;

    LPointerSwipeEndEvent pointerSwipeEndEvent;
    LPointerPinchEndEvent pointerPinchEndEvent;
    LPointerHoldEndEvent pointerHoldEndEvent;

    bool pointClippedByParent(LView *parent, const LPoint &point);
    bool pointClippedByParentScene(LView *view, const LPoint &point);
    LView *viewAt(LView *view, const LPoint &pos);

    inline LPointF viewLocalPos(LView *view, const LPointF &pos)
    {
        if ((view->scalingEnabled() || view->parentScalingEnabled()) && view->scalingVector().area() != 0.f)
            return (pos - view->pos()) / view->scalingVector();
        else
            return pos - view->pos();
    }

    inline bool pointerIsOverView(LView *view, const LPointF &pos)
    {
        if (!view->mapped() || !view->pointerEventsEnabled())
            return false;

        if (view->clippingEnabled() && !view->clippingRect().containsPoint(pos))
            return false;

        if (pointClippedByParent(view, pos))
            return false;

        if (pointClippedByParentScene(view, pos))
            return false;

        if ((view->scalingEnabled() || view->parentScalingEnabled()) && view->scalingVector() != LSizeF(1.f,1.f))
        {
            if (view->scalingVector().area() == 0.f)
                return false;

            if (view->inputRegion())
            {
                if (view->inputRegion()->containsPoint((pos - view->pos())/view->scalingVector()))
                    return true;
            }
            else
            {
                if (LRect(view->pos(), view->size()).containsPoint((pos - view->pos())/view->scalingVector()))
                    return true;
            }
        }
        else
        {
            if (view->inputRegion())
            {
                if (view->inputRegion()->containsPoint(pos - view->pos()))
                    return true;
            }
            else
            {
                if (LRect(view->pos(), view->size()).containsPoint(pos))
                    return true;
            }
        }

        return false;
    }

    bool handlePointerMove(LView *view);
};

#endif // LSCENEPRIVATE_H
