#ifndef LSCENE_H
#define LSCENE_H

#include <LObject.h>
#include <LPointer.h>
#include <LKeyboard.h>

/**
 * The LScene class is an optional utility that significantly simplifies rendering.
 * It encompasses a primary LSceneView which can host multiple children and even nested scenes.
 * A single LScene can drive multiple outputs and also manage pointer and keyboard Wayland events, while also providing per-view input events.
 * You might opt to leverage LScene for rendering instead of relying solely on the basic rendering functions from the LPainter class or your custom OpenGL shaders.
 *
 * LScene achieves high efficiency by rendering only damaged regions and avoiding rendering content obscured by opaque areas.
 *
 * Alternatively, you can still use your own OpenGL shaders or LPainter functions for rendering, either in conjunction with LScene or independently.
 * These approaches can be applied before or after handlePaintGL() is invoked, or by creating your custom LView that overrides the LView::paintRect() virtual method.
 *
 * ### Rendering
 *
 * For proper rendering with LScene, you need to "plug" the following methods into each LOutput you intend to manage with LScene:
 *
 * - handleInitializeGL()   -> LOutput::initializeGL()
 * - handlePaintGL()        -> LOutput::paintGL()
 * - handleMoveGL()         -> LOutput::moveGL()
 * - handleResizeGL()       -> LOutput::resizeGL()
 * - handleUninitializeGL() -> LOutput::uninitializeGL()
 *
 * @warning Make sure to "plug" the scene to all the output events mentioned earlier.
 *          Failing to do so may result in scene initialization issues, memory leaks, and potential compositor crashes.
 *
 * For example, like this:
 *
 * @code

#include <LOutput.h>

class YourCustomOutput : public LOutput
{
    LScene *scene;

    ...

    void initializeGL(LOutput *output) override
    {
        scene->handleInitializeGL(output);
    }

    void paintGL(LOutput *output) override
    {
        scene->handlePaintGL(output);
    }

    void moveGL(LOutput *output) override
    {
        scene->handleMoveGL(output);
    }

    void resizeGL(LOutput *output) override
    {
        scene->handleResizeGL(output);
    }

    void uninitializeGL(LOutput *output) override
    {
        scene->handleUninitializeGL(output);
    }
};

 * @endcode
 *
 * ### Pointer Events
 *
 * Similar to rendering, you can integrate the handlePointerXXX methods into the LPointer class.\n
 * This enables LScene to dispatch pointer events to each LView, invoking methods such as LView::pointerEnterEvent(), LView::pointerMoveEvent(), and more.
 * Additionally, the scene can manage pointer focus for LSurfaces and transmit pointer events accordingly to clients.
 * You also have the option to disable this behavior if you prefer to handle this logic independently.
 * To deactivate the handling of Wayland events, use the enableHandleWaylandPointerEvents() method.
 *
 * ### Keyboard Events
 *
 * Similar to rendering, you can integrate the handleKeyXXX methods into the LKeyboard class.\n
 * This empowers LScene to dispatch keyboard events to each LView, triggering methods like LView::keyEvent(), LView::keyModifiersEvent(), and more.
 * Furthermore, the scene can manage keyboard focus for LSurfaces and transmit keyboard events correspondingly to clients.
 * If desired, you can also opt to disable this behavior and manage the logic independently.
 * To deactivate the handling of Wayland events, utilize the enableHandleWaylandKeyboardEvents() method.
 */
class Louvre::LScene : public LObject
{
public:
    /**
     * @brief Default constructor for LScene.
     */
    LScene();

    /// @cond OMIT
    LCLASS_NO_COPY(LScene)
    /// @endcond

    /**
     * @brief Destructor for LScene.
     */
    ~LScene();

    /**
     * @brief Handle the OpenGL initialization for an LOutput.
     *
     * This method should be integrated into LOutput::initializeGL() to properly handle OpenGL initialization for the associated output.
     *
     * @param output The LOutput instance to handle OpenGL initialization for.
     */
    void handleInitializeGL(LOutput *output);

    /**
     * @brief Handle the OpenGL painting for an LOutput.
     *
     * This method should be integrated into LOutput::paintGL() to properly handle OpenGL painting for the associated output.
     *
     * @param output The LOutput instance to handle OpenGL painting for.
     */
    void handlePaintGL(LOutput *output);

    /**
     * @brief Handle the OpenGL movement for an LOutput.
     *
     * This method should be integrated into LOutput::moveGL() to properly handle OpenGL movement for the associated output.
     *
     * @param output The LOutput instance to handle OpenGL movement for.
     */
    void handleMoveGL(LOutput *output);

    /**
     * @brief Handle the OpenGL resizing for an LOutput.
     *
     * This method should be integrated into LOutput::resizeGL() to properly handle OpenGL resizing for the associated output.
     *
     * @param output The LOutput instance to handle OpenGL resizing for.
     */
    void handleResizeGL(LOutput *output);

    /**
     * @brief Handle the OpenGL uninitialization for an LOutput.
     *
     * This method should be integrated into LOutput::uninitializeGL() to properly handle OpenGL uninitialization for the associated output.
     *
     * @param output The LOutput instance to handle OpenGL uninitialization for.
     */
    void handleUninitializeGL(LOutput *output);

    /**
     * @brief Handle pointer movement event.
     *
     * This method should be integrated into LPointer::pointerMoveEvent() to effectively manage pointer movement events.
     *
     * @param event The pointer move event to handle.
     * @param outLocalPos Stores the local position of the first view found under the cursor. Pass `nullptr` if not needed.
     * @return The first LView found under the cursor.
     */
    LView *handlePointerMoveEvent(const LPointerMoveEvent &event, LPointF *outLocalPos = nullptr);

    /**
     * @brief Handle pointer button event.
     *
     * This method should be integrated into LPointer::pointerButtonEvent() to handle pointer button events.
     *
     * @param event The pointer button event to handle.
     */
    void handlePointerButtonEvent(const LPointerButtonEvent &event);

    /**
     * @brief Handle pointer scroll event.
     *
     * This method should be integrated into LPointer::pointerScrollEvent() to manage pointer scroll events.
     *
     * @param event The pointer scroll event to handle.
     */
    void handlePointerScrollEvent(const LPointerScrollEvent &event);

    /**
     * @brief Check if handling of Wayland pointer events is enabled.
     *
     * This method allows you to check whether the handling of Wayland pointer events is currently enabled.
     *
     * @return True if Wayland pointer events handling is enabled, false otherwise.
     */
    bool handleWaylandPointerEventsEnabled() const;

    /**
     * @brief Enable or disable handling of Wayland pointer events.
     *
     * This method enables or disables the handling of Wayland pointer events by LScene.
     *
     * @param enabled True to enable handling of Wayland pointer events, false to disable.
     */
    void enableHandleWaylandPointerEvents(bool enabled);

    /**
     * @brief Handle key event.
     *
     * This method should be integrated into LKeyboard::keyEvent() to handle key events.
     */
    void handleKeyEvent(const LKeyboardKeyEvent &event);

    /**
     * @brief Check if handling of Wayland keyboard events is enabled.
     *
     * This method allows you to check whether the handling of Wayland keyboard events is currently enabled.
     *
     * @return True if Wayland keyboard events handling is enabled, false otherwise.
     */
    bool handleWaylandKeyboardEventsEnabled() const;

    /**
     * @brief Enable or disable handling of Wayland keyboard events.
     *
     * This method enables or disables the handling of Wayland keyboard events by LScene.
     *
     * @param enabled True to enable handling of Wayland keyboard events, false to disable.
     */
    void enableHandleWaylandKeyboardEvents(bool enabled);

    /**
     * @brief Check if auxiliary keyboard implementation is enabled.
     *
     * This method allows you to check whether auxiliary keyboard functions, such as closing the compositor with Ctrl + Alt + Esc,
     * executing `weston-terminal` with F1, or modifying the drag-and-drop action with Shift, Alt, or Ctrl keys, are currently enabled.
     *
     * @return True if auxiliary keyboard implementation is enabled, false otherwise.
     */
    bool auxKeyboardImplementationEnabled() const;

    /**
     * @brief Enable or disable auxiliary keyboard implementation.
     *
     * This method enables or disables auxiliary keyboard functions, such as custom key actions.
     *
     * @param enabled True to enable auxiliary keyboard implementation, false to disable.
     */
    void enableAuxKeyboardImplementation(bool enabled);

    /**
     * @brief Retrieve the main view of the scene.
     *
     * This method returns the main LSceneView associated with the LScene.
     *
     * @return A pointer to the main LSceneView of the scene.
     */
    LSceneView *mainView() const;

    /**
     * @brief Retrieve the view located at the specified position.
     *
     * This method returns the LView instance that occupies the given position within the scene.
     *
     * @param pos The position to query.
     * @return A pointer to the LView at the specified position, or nullptr if no view is found.
     */
    LView *viewAt(const LPoint &pos);

LPRIVATE_IMP_UNIQUE(LScene)
};

#endif // LSCENE_H
