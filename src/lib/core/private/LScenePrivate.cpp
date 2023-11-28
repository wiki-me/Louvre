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

LView *LScene::LScenePrivate::viewAt(LView *view, const LPoint &pos)
{
    LView *v = nullptr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
    {
        v = viewAt(*it, pos);

        if (v)
            return v;
    }

    if (!view->mapped() || !view->inputEnabled())
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

    if (!pointerIsBlocked && pointerIsOverView(view, currentPointerMoveEvent.pos()))
    {
        if (!(pointerMoveEventFirstView))
            pointerMoveEventFirstView = view;

        if (!(view->imp()->state & LVS::PointerMoveDone))
        {
            view->imp()->state |= LVS::PointerMoveDone;

            if (view->pointerIsOver())
            {
                view->pointerMoveEvent(currentPointerMoveEvent);

                if (listChanged)
                    goto listChangedErr;
            }
            else
            {
                view->imp()->addFlag(LVS::PointerIsOver);
                view->pointerEnterEvent(currentPointerMoveEvent);

                if (listChanged)
                    goto listChangedErr;
            }
        }

        if (view->blockPointerEnabled())
            pointerIsBlocked = true;
    }
    else
    {
        if (!(view->imp()->state & LVS::PointerMoveDone))
        {
            view->imp()->state |= LVS::PointerMoveDone;

            if (view->pointerIsOver())
            {
                view->imp()->removeFlag(LVS::PointerIsOver);
                view->pointerLeaveEvent(currentPointerMoveEvent);

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

bool LScene::LScenePrivate::handlePointerButton(LView *view)
{
    if (listChanged)
        goto listChangedErr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
        if (!handlePointerButton(*it))
            return false;

    if (view->imp()->state & LVS::PointerButtonDone)
        return true;

    view->imp()->state |= LVS::PointerButtonDone;

    if (view->imp()->hasFlag(LVS::PointerIsOver))
        view->pointerButtonEvent(currentPointerButtonEvent);

    if (listChanged)
        goto listChangedErr;

    return true;

    // If a list was modified, start again, serials are used to prevent resend events
    listChangedErr:
    listChanged = false;
    handlePointerButton(&this->view);
    return false;
}

bool LScene::LScenePrivate::handlePointerScrollEvent(LView *view)
{
    if (listChanged)
        goto listChangedErr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
        if (!handlePointerScrollEvent(*it))
            return false;

    if (view->imp()->state & LVS::PointerScrollDone)
        return true;

    view->imp()->state |= LVS::PointerScrollDone;

    if (view->imp()->hasFlag(LVS::PointerIsOver))
        view->pointerScrollEvent(currentPointerScrollEvent);

    if (listChanged)
        goto listChangedErr;

    return true;

    // If a list was modified, start again, serials are used to prevent resend events
    listChangedErr:
    listChanged = false;
    handlePointerScrollEvent(&this->view);
    return false;
}

bool LScene::LScenePrivate::handleKeyEvent(LView *view)
{
    if (listChanged)
        goto listChangedErr;

    for (std::list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
        if (!handleKeyEvent(*it))
            return false;

    if (view->imp()->state & LVS::KeyDone)
        return true;

    view->imp()->state |= LVS::KeyDone;

    view->keyEvent(currentKeyboardKeyEvent);

    if (listChanged)
        goto listChangedErr;

    return true;

    // If a list was modified, start again, serials are used to prevent resend events
    listChangedErr:
    listChanged = false;
    handleKeyEvent(&this->view);
    return false;
}
