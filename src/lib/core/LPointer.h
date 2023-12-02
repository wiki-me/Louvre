#ifndef LPOINTER_H
#define LPOINTER_H

#include <LObject.h>
#include <LPoint.h>
#include <LToplevelRole.h>
#include <LPointerButtonEvent.h>
#include <linux/input-event-codes.h>
#include <limits>

/**
 * @brief Class for handling events generated by pointing devices.
 *
 * The LPointer class allows you to handle pointer events generated by devices such as a mouse or touchpad.\n
 * It provides utility methods for interactive tasks like moving and resizing toplevel surfaces, sending pointer events to client surfaces, and more.\n
 * There is always a single instance of LPointer, which can be accessed through LSeat::pointer().
 *
 * ### Wayland Events
 *
 * To send pointer events to clients, you must first assign focus to a surface using one of the setFocus() variants.\n
 * This action automatically removes focus from any previously focused surface.
 * Subsequently, all pointer events are directed to the currently focused surface.
 */
class Louvre::LPointer : public LObject
{
public:
    /**
     * @brief Constructor parameters.
     *
     * Configuration parameters provided in the virtual LCompositor::createPointerRequest() virtual constructor.
     */
    struct Params;

    /**
     * @brief LPointer class constructor.
     *
     * There is a single instance of LPointer, which can be accessed from LSeat::pointer().
     *
     * @param params Internal library parameters provided in the LCompositor::createPointerRequest() virtual constructor.
     */
    LPointer(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LPointer)
    /// @endcond

    /**
     * @brief LPointer class destructor.
     *
     * Invoked internally by the library after notification of its destruction with LCompositor::destroyPointerRequest().
     */
    virtual ~LPointer();

    /**
     * @brief Get the surface with pointer focus.
     *
     * This method returns the surface that has been assigned pointer focus using setFocus().\n
     * Only surfaces with pointer focus can receive pointer events.
     *
     * @return A pointer to the focused surface, or `nullptr` if no surface has pointer focus.
     */
    LSurface *focus() const;

    /**
     * @brief Keep track of the surface pressed by the left pointer button.
     *
     * This is just a utility method used by the default LPointer implementation to ensure that pointer focus remains
     * on a surface while it's being actively pressed by the left pointer button, for example, during text selection, even
     * if the pointer moves outside the surface boundaries.\n
     * The draggingSurface() property is automatically set to `nullptr` if the given surface is destroyed.
     *
     * @note This method is unrelated to drag & drop sessions.
     *
     * @param surface The surface being pressed or `nullptr` to unset.
     *
     * @see draggingSurface()
     */
    void setDraggingSurface(LSurface *surface);

    /**
     * @brief Surface being actively pressed by the left pointer button.
     *
     * This method returns the surface that is currently being actively pressed by the left pointer button.
     * It is set using the setDraggingSurface() utility method to ensure that pointer focus remains on the surface
     * even if the pointer moves outside its boundaries during interaction.
     *
     * @returns The surface being pressed or `nullptr`.
     *
     * @see setDraggingSurface()
     */
    LSurface *draggingSurface() const;

    /**
     * @brief Look for a surface.
     *
     * This method looks for the first mapped surface that contains the point given by the `point` parameter.\n
     * It takes into account the surfaces role position (LSurface::rolePos()), their input region (LSurface::inputRegion()) and the order
     * given by the list of surfaces of the compositor (LCompositor::surfaces()).\n
     * Some surface roles do not have an input region such as LCursorRole or LDNDIconRole so these surfaces are always ignored.
     *
     * @param point Point in compositor coordinates.
     * @returns Returns the first surface that contains the point or `nullptr` if no surface is found.
     */
    LSurface *surfaceAt(const LPoint &point);

    /**
     * @brief Retrieve the last LCursorRole provided in the most recent setCursorRequest() call.
     *
     * If the cursor role has been destroyed or if a client has requested to hide the cursor, this method returns `nullptr`.
     *
     * @return A pointer to the last LCursorRole, or `nullptr` if the cursor role has been destroyed or the client requested to hide the cursor.
     *
     * @note To distinguish whether the last cursor was destroyed or the request was to hide it, use the lastCursorRequestWasHide() method.
     */
    LCursorRole *lastCursorRequest() const;

    /**
     * @brief Determine if the intention of the most recent setCursorRequest() was to hide the cursor.
     *
     * This method returns `true` if the most recent setCursorRequest() was intended to hide the cursor; otherwise, it returns `false`.
     *
     * @return `true` if the last cursor request was to hide the cursor; otherwise, `false`.
     */
    bool lastCursorRequestWasHide() const;

    /**
     * @brief Close all popups.
     *
     * This method closes all active Popup surfaces in reverse order of creation.
     *
     * @see LPopupRole::sendPopupDoneEvent()
     */
    void dismissPopups();

    /**
     * @name Client Events
     *
     * These methods allow you to send pointer events to clients.
     *
     * @note Pointer events are sent to the currently focused surface set with setFocus(). If no surface has focus, calling these methods has no effect.
     */

///@{

    /**
     * @brief Assign or remove pointer focus.
     *
     * This method sets the pointer focus to the provided surface based on the current LCursor position.\n
     * If a surface already had pointer focus, it will lose it.
     *
     * Passing `nullptr` removes pointer focus from all surfaces.
     *
     * @note This method internally transforms the LCursor position to the local coordinates of the focused surface,
     *       taking into account the surface's role position.
     *
     * @param surface The surface to which you want to assign the pointer focus or `nullptr` to remove focus from all surfaces.
     */
    void setFocus(LSurface *surface);

    /**
     * @brief Assign or remove pointer focus.
     *
     * This method assigns the pointer focus to the specified surface at the given local position within the surface.\n
     * If another surface already has pointer focus, it will lose it.
     *
     * Passing `nullptr` removes pointer focus from all surfaces.
     *
     * @param surface Surface to which the pointer focus will be assigned, or `nullptr` to remove focus from all surfaces.
     * @param localPos Local position within the surface where the pointer enters.
     */
    void setFocus(LSurface *surface, const LPointF &localPos);

    /**
     * @brief Send a pointer move event.
     *
     * This method sends the current pointer position to the surface with pointer focus.
     * If no surface has pointer focus calling this method is a no-op.
     *
     * The the position must be in surface local coordinates and is stracted from the
     * LPointerMoveEvent::localPos mutable variable of the event. Make sure to assign its
     * value properly.
     */
    void sendMoveEvent(const LPointerMoveEvent &event);

    /**
     * @brief Send a pointer button event.
     *
     * This method sends a pointer button event to the surface with focus
     */
    void sendButtonEvent(const LPointerButtonEvent &event);

    /**
     * @brief Send a scroll event.
     *
     * This method sends a scroll event to the currently focused surface.
     */
    void sendScrollEvent(const LPointerScrollEvent &event);

///@}

    /**
     * @name Virtual Methods
     */

///@{

    /**
     * @brief Pointer move event.
     *
     * This virtual method notifies of pointer movement generated by the input backend.\n
     * Override this virtual method if you need to be informed when the pointer changes its position.
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp pointerMoveEvent
     */
    virtual void pointerMoveEvent(const LPointerMoveEvent &event);

    /**
     * @brief Pointer button event.
     *
     * This virtual method notifies of a change in the state of a pointer button generated by the input backend.\n
     * Override this virtual method if you need to be informed when a pointer button changes its state.
     *
     * @param button The code of the pressed button.
     * @param state The state of the button (LPointer::Pressed or LPointer::Released).
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp pointerButtonEvent
     */
    virtual void pointerButtonEvent(const LPointerButtonEvent &event);

    /**
     * @brief Pointer scroll event.
     *
     * This virtual method notifies about a pointer scroll event generated by the input backend.\n
     * Override this virtual method if you need to handle scroll events.
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp pointerAxisEvent
     */
    virtual void pointerScrollEvent(const LPointerScrollEvent &event);

    /**
     * @brief Set cursor request.
     *
     * This virtual method is triggered when a client requests to set the cursor texture and hotspot or hide it.\n
     * If you want to assign the cursor texture as requested by the client, override this method.
     *
     * @note Only clients with a focused surface can request to set the cursor texture.
     *
     * @param cursorRole Surface role to use as a cursor. If it is `nullptr`, it indicates that the client wants to hide the cursor.
     *
     * @see lastCursorRequest()
     * @see lastCursorRequestWasHide()
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp setCursorRequest
     */
    virtual void setCursorRequest(LCursorRole *cursorRole);

///@}

    LPRIVATE_IMP_UNIQUE(LPointer)
};

#endif // LPOINTER_H
