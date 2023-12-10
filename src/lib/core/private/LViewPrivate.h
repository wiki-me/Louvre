#ifndef LVIEWPRIVATE_H
#define LVIEWPRIVATE_H

#include <private/LScenePrivate.h>
#include <LRegion.h>
#include <LView.h>
#include <LRect.h>
#include <map>
#include <thread>
#include <LPainter.h>
#include <GL/gl.h>

using namespace Louvre;

LPRIVATE_CLASS(LView)

    enum LViewState : UInt32
    {
        IsScene                 = 1 << 0,

        PointerEvents           = 1 << 1,
        KeyboardEvents          = 1 << 2,
        TouchEvents             = 1 << 3,

        BlockPointer            = 1 << 4,

        RepaintCalled           = 1 << 5,
        ColorFactor             = 1 << 6,
        Visible                 = 1 << 7,
        Scaling                 = 1 << 8,
        ParentScaling           = 1 << 9,
        ParentOffset            = 1 << 10,
        Clipping                = 1 << 11,
        ParentClipping          = 1 << 12,
        ParentOpacity           = 1 << 13,
        ForceRequestNextFrame   = 1 << 14,
    };

    /* Sometimes view ordering can change while a scene is emitting an event,
     * in such cases it must start again. These flags are for preventing re-sending
     * the event to the same view */
    enum LViewEventsState : UInt32
    {
        PointerIsOver           = 1 << 0,

        PendingSwipeEnd         = 1 << 1,
        PendingPinchEnd         = 1 << 2,
        PendingHoldEnd          = 1 << 3,

        PointerMoveDone         = 1 << 4,
        PointerButtonDone       = 1 << 5,
        PointerScrollDone       = 1 << 6,
        PointerSwipeBeginDone   = 1 << 7,
        PointerSwipeUpdateDone  = 1 << 8,
        PointerSwipeEndDone     = 1 << 9,
        PointerPinchBeginDone   = 1 << 10,
        PointerPinchUpdateDone  = 1 << 11,
        PointerPinchEndDone     = 1 << 12,
        PointerHoldBeginDone    = 1 << 13,
        PointerHoldEndDone      = 1 << 14,
        KeyDone                 = 1 << 15,
        TouchDownDone           = 1 << 16,
        TouchMoveDone           = 1 << 17,
        TouchUpDone             = 1 << 18,
        TouchFrameDone          = 1 << 19,
        TouchCancelDone         = 1 << 20,
    };

    // This is used for detecting changes on a view since the last time it was drawn on a specific output
    struct ViewThreadData
    {
        LOutput *o = nullptr;
        Float32 prevOpacity = 1.f;
        UInt32 lastRenderedDamageId = 0;
        LRect prevRect;
        LRect prevLocalRect;
        bool changedOrder = true;
        bool prevMapped = false;
        LRegion prevClipping;
        LRGBAF prevColorFactor;
        bool prevColorFactorEnabled = false;
    };

    // This is used to prevent invoking heavy methods
    struct ViewCache
    {
        ViewThreadData *voD;
        LRect rect;
        LRect localRect;
        LRegion damage;
        LRegion translucent;
        LRegion opaque;
        LRegion opaqueOverlay;
        Float32 opacity;
        LSizeF scalingVector;
        bool mapped = false;
        bool occluded = false;
        bool scalingEnabled;
        bool isFullyTrans;
    };

    UInt32 state = Visible | ParentOffset | ParentOpacity | BlockPointer;
    UInt32 eventsState = 0;
    ViewCache cache;

    UInt32 type;
    LView *parent = nullptr;
    LView *view = nullptr;
    std::list<LView*>children;
    GLenum sFactor = GL_SRC_ALPHA;
    GLenum dFactor = GL_ONE_MINUS_SRC_ALPHA;
    LRGBAF colorFactor = {1.f, 1.f, 1.f, 1.f};
    Float32 opacity = 1.f;
    LSizeF scalingVector = LSizeF(1.f, 1.f);
    LRect clippingRect;
    LPoint tmpPoint;
    LSize tmpSize;
    LPointF tmpPointF;

    std::map<std::thread::id,ViewThreadData>threadsMap;
    LScene *currentScene = nullptr;
    std::list<LView*>::iterator parentLink;
    std::list<LView*>::iterator compositorLink;

    std::list<LView*>::iterator pointerLink;
    std::list<LView*>::iterator keyboardLink;
    std::list<LView*>::iterator touchLink;

    void removeThread(Louvre::LView *view, std::thread::id thread);
    void markAsChangedOrder(bool includeChildren = true);
    void damageScene(LSceneView *s);

    inline void removeFlag(LViewState flag)
    {
        state &= ~flag;
    }

    inline void addFlag(LViewState flag)
    {
        state |= flag;
    }

    inline bool hasFlag(LViewState flag)
    {
        return state & flag;
    }

    inline void setFlag(LViewState flag, bool enable)
    {
        if (enable)
            addFlag(flag);
        else
            removeFlag(flag);
    }

    inline static void removeFlagWithChildren(LView *view, LViewState flag)
    {
        view->imp()->removeFlag(flag);

        for (LView *child : view->imp()->children)
            removeFlagWithChildren(child, flag);
    }

    inline void removeEventFlag(LViewEventsState flag)
    {
        eventsState &= ~flag;
    }

    inline void addEventFlag(LViewEventsState flag)
    {
        eventsState |= flag;
    }

    inline bool hasEventFlag(LViewEventsState flag)
    {
        return eventsState & flag;
    }

    inline void setEventFlag(LViewEventsState flag, bool enable)
    {
        if (enable)
            addEventFlag(flag);
        else
            removeEventFlag(flag);
    }

    inline static void removeEventFlagWithChildren(LView *view, LViewEventsState flag)
    {
        view->imp()->removeEventFlag(flag);

        for (LView *child : view->imp()->children)
            removeEventFlagWithChildren(child, flag);
    }

    inline void sceneChanged(LScene *newScene)
    {
        if (currentScene)
        {
            if (hasFlag(KeyboardEvents))
            {
                currentScene->imp()->keyboardFocus.erase(keyboardLink);
                currentScene->imp()->keyboardListChanged = true;
            }

            if (hasEventFlag(PointerIsOver))
            {
                if (hasEventFlag(PendingSwipeEnd))
                {
                    removeEventFlag(PendingSwipeEnd);
                    currentScene->imp()->pointerSwipeEndEvent.setCancelled(true);
                    currentScene->imp()->pointerSwipeEndEvent.setMs(currentScene->imp()->currentPointerMoveEvent.ms());
                    currentScene->imp()->pointerSwipeEndEvent.setUs(currentScene->imp()->currentPointerMoveEvent.us());
                    currentScene->imp()->pointerSwipeEndEvent.setSerial(LTime::nextSerial());
                    view->pointerSwipeEndEvent(currentScene->imp()->pointerSwipeEndEvent);
                }

                if (hasEventFlag(PendingPinchEnd))
                {
                    removeEventFlag(PendingPinchEnd);
                    currentScene->imp()->pointerPinchEndEvent.setCancelled(true);
                    currentScene->imp()->pointerPinchEndEvent.setMs(currentScene->imp()->currentPointerMoveEvent.ms());
                    currentScene->imp()->pointerPinchEndEvent.setUs(currentScene->imp()->currentPointerMoveEvent.us());
                    currentScene->imp()->pointerPinchEndEvent.setSerial(LTime::nextSerial());
                    view->pointerPinchEndEvent(currentScene->imp()->pointerPinchEndEvent);
                }

                if (hasEventFlag(PendingHoldEnd))
                {
                    removeEventFlag(PendingHoldEnd);
                    currentScene->imp()->pointerHoldEndEvent.setCancelled(true);
                    currentScene->imp()->pointerHoldEndEvent.setMs(currentScene->imp()->currentPointerMoveEvent.ms());
                    currentScene->imp()->pointerHoldEndEvent.setUs(currentScene->imp()->currentPointerMoveEvent.us());
                    currentScene->imp()->pointerHoldEndEvent.setSerial(LTime::nextSerial());
                    view->pointerHoldEndEvent(currentScene->imp()->pointerHoldEndEvent);
                }

                currentScene->imp()->pointerFocus.erase(pointerLink);
                currentScene->imp()->pointerListChanged = true;
                removeEventFlag(PointerIsOver);
            }
        }

        if (newScene)
        {
            if (hasFlag(KeyboardEvents))
            {
                newScene->imp()->keyboardFocus.push_back(view);
                keyboardLink = std::prev(newScene->imp()->keyboardFocus.end());
                newScene->imp()->keyboardListChanged = true;
            }
        }

        currentScene = newScene;

        for (LView *child : children)
            child->imp()->sceneChanged(newScene);
    }
};

#endif // LVIEWPRIVATE_H
