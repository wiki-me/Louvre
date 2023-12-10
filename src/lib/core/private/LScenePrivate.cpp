#include "LCursor.h"
#include <private/LScenePrivate.h>
#include <private/LViewPrivate.h>
#include <private/LSceneViewPrivate.h>
#include <LOutput.h>
#include <LCompositor.h>
#include <LPainter.h>
#include <LSurfaceView.h>
#include <LFramebuffer.h>
#include <LLog.h>

using LVS = LView::LViewPrivate::LViewState;
using LVES = LView::LViewPrivate::LViewEventsState;

LView *LScene::LScenePrivate::viewAt(LView *view, const LPoint &pos)
{
    LView *v = nullptr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
    {
        v = viewAt(*it, pos);

        if (v)
            return v;
    }

    if (!view->mapped() || !view->pointerEventsEnabled())
        return nullptr;

    if (view->clippingEnabled() && !view->clippingRect().containsPoint(pos))
        return nullptr;

    if (pointClippedByParent(view, pos))
        return nullptr;

    if (pointClippedByParentScene(view, pos))
        return nullptr;

    if ((view->scalingEnabled() || view->parentScalingEnabled()) && view->scalingVector() != LSizeF(1.f,1.f))
    {
        if (view->scalingVector().area() == 0.f)
            return nullptr;

        if (view->inputRegion())
        {
            if (view->inputRegion()->containsPoint((pos - view->pos())/view->scalingVector()))
                v = view;
        }
        else
        {
            if (LRect(view->pos(), view->size()).containsPoint((pos - view->pos())/view->scalingVector()))
                v = view;
        }
    }
    else
    {
        if (view->inputRegion())
        {
            if (view->inputRegion()->containsPoint(pos - view->pos()))
                v = view;
        }
        else
        {
            if (LRect(view->pos(), view->size()).containsPoint(pos))
                v = view;
        }
    }

    return v;
}

bool LScene::LScenePrivate::pointClippedByParent(LView *view, const LPoint &point)
{
    if (!view->parent())
        return false;

    if (view->parentClippingEnabled())
    {
        if (!LRect(view->parent()->pos(), view->parent()->size()).containsPoint(point))
            return true;
    }

    return pointClippedByParent(view->parent(), point);
}

bool LScene::LScenePrivate::pointClippedByParentScene(LView *view, const LPoint &point)
{
    LSceneView *parentScene = view->parentSceneView();

    if (!parentScene)
        return false;

    if (parentScene->isLScene())
        return false;

    if (!parentScene->imp()->fb->rect().containsPoint(point))
        return true;

    return pointClippedByParentScene(parentScene, point);
}

bool LScene::LScenePrivate::handlePointerMove(LView *view)
{
    if (listChanged)
        goto listChangedErr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
        if (!handlePointerMove(*it))
            return false;

    if (!pointerIsBlocked && pointerIsOverView(view, cursor()->pos()))
    {
        if (!view->imp()->hasEventFlag(LVES::PointerMoveDone))
        {
            view->imp()->addEventFlag(LVES::PointerMoveDone);

            if (view->imp()->hasEventFlag(LVES::PointerIsOver))
            {
                pointerFocus.erase(view->imp()->pointerLink);
                pointerFocus.push_back(view);
                view->imp()->pointerLink = std::prev(pointerFocus.end());
                currentPointerMoveEvent.localPos = viewLocalPos(view, cursor()->pos());
                view->pointerMoveEvent(currentPointerMoveEvent);

                if (listChanged)
                    goto listChangedErr;
            }
            else
            {
                view->imp()->addEventFlag(LVES::PointerIsOver);
                pointerFocus.push_back(view);
                view->imp()->pointerLink = std::prev(pointerFocus.end());
                currentPointerEnterEvent.localPos = viewLocalPos(view, cursor()->pos());
                view->pointerEnterEvent(currentPointerEnterEvent);

                if (listChanged)
                    goto listChangedErr;
            }
        }

        if (view->blockPointerEnabled())
            pointerIsBlocked = true;
    }
    else
    {
        if (!view->imp()->hasEventFlag(LVES::PointerMoveDone))
        {
            view->imp()->addEventFlag(LVES::PointerMoveDone);

            if (view->imp()->hasEventFlag(LVES::PointerIsOver))
            {
                view->imp()->removeEventFlag(LVES::PointerIsOver);

                if (view->imp()->hasEventFlag(LVES::PendingSwipeEnd))
                {
                    view->imp()->removeEventFlag(LVES::PendingSwipeEnd);
                    pointerSwipeEndEvent.setCancelled(true);
                    pointerSwipeEndEvent.setMs(currentPointerMoveEvent.ms());
                    pointerSwipeEndEvent.setUs(currentPointerMoveEvent.us());
                    pointerSwipeEndEvent.setSerial(LTime::nextSerial());
                    view->pointerSwipeEndEvent(pointerSwipeEndEvent);
                }

                if (view->imp()->hasEventFlag(LVES::PendingPinchEnd))
                {
                    view->imp()->removeEventFlag(LVES::PendingPinchEnd);
                    pointerPinchEndEvent.setCancelled(true);
                    pointerPinchEndEvent.setMs(currentPointerMoveEvent.ms());
                    pointerPinchEndEvent.setUs(currentPointerMoveEvent.us());
                    pointerPinchEndEvent.setSerial(LTime::nextSerial());
                    view->pointerPinchEndEvent(pointerPinchEndEvent);
                }

                if (view->imp()->hasEventFlag(LVES::PendingHoldEnd))
                {
                    view->imp()->removeEventFlag(LVES::PendingHoldEnd);
                    pointerHoldEndEvent.setCancelled(true);
                    pointerHoldEndEvent.setMs(currentPointerMoveEvent.ms());
                    pointerHoldEndEvent.setUs(currentPointerMoveEvent.us());
                    pointerHoldEndEvent.setSerial(LTime::nextSerial());
                    view->pointerHoldEndEvent(pointerHoldEndEvent);
                }

                pointerFocus.erase(view->imp()->pointerLink);
                view->pointerLeaveEvent(currentPointerLeaveEvent);

                if (listChanged)
                    goto listChangedErr;
            }
        }
    }

    return true;

    // If a list was modified, start again, serials are used to prevent resend events
    listChangedErr:
    listChanged = false;
    handlePointerMove(&this->view);
    return false;
}
