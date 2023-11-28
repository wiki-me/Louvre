#ifndef LSCENEPRIVATE_H
#define LSCENEPRIVATE_H

#include <LRegion.h>
#include <LPointerMoveEvent.h>
#include <LPointerButtonEvent.h>
#include <LPointerScrollEvent.h>
#include <LKeyboardKeyEvent.h>
#include <LSceneView.h>
#include <LScene.h>
#include <mutex>

using namespace Louvre;

LPRIVATE_CLASS(LScene)
    std::mutex mutex;
    LSceneView view;
    bool handleWaylandPointerEvents = true;
    bool handleWaylandKeyboardEvents = true;
    bool auxKeyboardImplementationEnabled = true;
    bool pointerIsBlocked = false;

    bool listChanged = false;

    // Prevent recursive calls
    bool handlingPointerMove = false;
    bool handlingPointerButton = false;
    bool handlingPointerScrollEvent = false;
    bool handlingKeyEvent = false;

    LView *pointerMoveEventFirstView;
    LPointF pointerMoveEventOutLocalPos;
    LPointerMoveEvent currentPointerMoveEvent;
    LPointerButtonEvent currentPointerButtonEvent;
    LPointerScrollEvent currentPointerScrollEvent;
    LKeyboardKeyEvent currentKeyboardKeyEvent;

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
        if (!view->mapped() || !view->inputEnabled())
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
    bool handlePointerButton(LView *view);
    bool handlePointerScrollEvent(LView *view);
    bool handleKeyEvent(LView *view);
};

#endif // LSCENEPRIVATE_H
