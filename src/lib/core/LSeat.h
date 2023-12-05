#ifndef LSEAT_H
#define LSEAT_H

#include <LObject.h>
#include <LToplevelRole.h>
#include <LSurface.h>

struct libseat;

/**
 * @brief Group of input and output devices.
 *
 * The LSeat class represents a collection of input and output devices such as a mouse, keyboard,
 * and outputs (screens). These devices are used within a session. Typically, access to these devices is restricted to a single process per session,
 * often a Wayland or X11 compositor.
 *
 * To enable input and graphics backends to manage seat devices without requiring superuser privileges, the openDevice()
 * method can be used to obtain device file descriptors. This method internally employs [libseat](https://github.com/kennylevinsen/seatd) to request seat permissions.
 *
 * By setting the environment variable **LOUVRE_ENABLE_LIBSEAT** to 0, [libseat](https://github.com/kennylevinsen/seatd) can be disabled, causing the compositor to launch without multi-seat support.
 * Consequently, certain features such as switching to another TTY may become unavailable.\n
 *
 * @note Disabling [libseat](https://github.com/kennylevinsen/seatd) may be necessary in scenarios where the compositor needs to be started remotely via SSH.
 *
 * The LSeat class also facilitates tasks such as session (TTY) switching, clipboard access, interaction with instances of LPointer and LKeyboard, as well as native events
 * monitoring from the input backend.
 *
 * @warning Touch events have not been implemented yet.
 */
class Louvre::LSeat : public LObject
{
public:

    const std::list<LToplevelResizeSession*> &resizeSessions() const;

    struct Params;
    using InputCapabilitiesFlags = UInt32;

    /**
     * @brief Input capabilities
     *
     * Compositor input capabilities.\n
     */
    enum InputCapabilities : InputCapabilitiesFlags
    {
        /// Pointer events
        Pointer = 1,

        /// Keyboard events
        Keyboard = 2,

        /// Touch events
        Touch = 4
    };

    /**
     * @brief LSeat class constructor.
     *
     * There is only one instance of LSeat, which can be accessed from LCompositor::seat().
     *
     * @param params Internal library parameters provided in the virtual LCompositor::createSeatRequest() constructor.
     */
    LSeat(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LSeat)
    /// @endcond

    /**
     * @brief LSeat class destructor.
     *
     * Invoked after LCompositor::destroySeatRequest().
     */
    virtual ~LSeat();

    /**
     * @brief List of available outputs.
     *
     * This method provides a list of currently available outputs. The list includes connected outputs that can be initialized
     * as well as those that are already initialized.
     * To obtain a list of only initialized outputs, refer to LCompositor::outputs().
     */
    const std::list<LOutput *> &outputs() const;

    /**
     * @brief List of avaliable input devices
     */
    const std::list<LInputDevice*> &inputDevices() const;

    /**
     * @brief Handle to the native context used by the graphic backend.
     *
     * This opaque handle is unique to each graphic backend.\n
     * In the case of the DRM backend, it returns a pointer to a [SRMCore](https://cuarzosoftware.github.io/SRM/group___s_r_m_core.html) struct.\n
     * In the case of the X11 backend, it returns a pointer to a [Display](https://www.x.org/releases/X11R7.6/doc/libX11/specs/libX11/libX11.html) struct.\n
     *
     * You can use this handle to configure specific aspects of each backend.
     */
    void *graphicBackendContextHandle() const;

    /**
     * @brief Get the ID of the current graphic backend.
     *
     * Each graphic backend is assigned a unique Louvre::UInt32 ID. You can use this method to retrieve the
     * ID of the current graphic backend in use.
     *
     * The IDs of graphic backends shipped with Louvre are listed in the Louvre::LGraphicBackendID enum.
     *
     * @return The ID of the graphic backend.
     */
    UInt32 graphicBackendId() const;

    /**
     * @brief Handle to the native context used by the input backend.
     *
     * This opaque handle is unique to each input backend.\n
     * In the case of the Libinput backend, it returns a pointer to a [libinput](https://wayland.freedesktop.org/libinput/doc/latest/api/structlibinput.html) struct.\n
     * In the case of the X11 backend, it returns a pointer to a [Display](https://www.x.org/releases/X11R7.6/doc/libX11/specs/libX11/libX11.html) struct.\n
     *
     * You can use this handle to configure specific aspects of each backend.
     */
    void *inputBackendContextHandle() const;

    /**
     * @brief Get the ID of the current input backend.
     *
     * Each input backend is assigned a unique Louvre::UInt32 ID. You can use this method to retrieve the
     * ID of the current input backend in use.
     *
     * The IDs of input backends shipped with Louvre are listed in the Louvre::LInputBackendID enum.
     *
     * @return The ID of the input backend.
     */
    UInt32 inputBackendId() const;

    /**
     * @brief Retrieve the input backend capabilities.
     *
     * This method returns flags representing the input capabilities of the input backend.
     * These capabilities are defined in Louvre::LSeat::InputCapabilities and may change each time an input device is plugged or unplugged.
     *
     * @return Input capabilities flags of the input backend.
     */
    InputCapabilitiesFlags inputBackendCapabilities() const;

    /**
     * @brief The seat name
     *
     * This method returns the name of the seat (E.g. "seat0").
     */
    const char *name() const;

    /**
     * @brief Retrieve the input capabilities of the compositor.
     *
     * This method returns flags representing the input capabilities assigned to the compositor using setInputCapabilities().
     * These capabilities may differ from those of the input backend.
     *
     * @return Input capabilities flags assigned to the compositor.
     */
    InputCapabilitiesFlags inputCapabilities() const;

    /**
     * @brief Set the input capabilities of the compositor.
     *
     * This method informs clients about the compositor's input capabilities.
     * Clients will only listen to events specified in the capabilities flags.
     * The default implementation of initialized() sets the compositor's capabilities to match those of the input backend.
     *
     * @param capabilitiesFlags Flags representing the input capabilities of the compositor, as defined in Louvre::LSeat::InputCapabilities. These capabilities may differ from those of the input backend.
     */
    void setInputCapabilities(InputCapabilitiesFlags capabilitiesFlags);

    /**
     * @brief Active toplevel surface.
     *
     * This method returns the active LToplevelRole role assigned by passing the flag Louvre::LToplevelRole::Activated in Louvre::LToplevelRole::confgure().\n
     * Only one toplevel surface can be active at a time, the library automatically deactivates other Toplevels when one is activated.
     *
     * @return The currently active LToplevelRole instance or `nullptr` if no toplevel is active.
     */
    LToplevelRole *activeToplevel() const;

    /**
     * @brief Access to pointer events.
     *
     * Access to the LPointer instance used to receive pointer events from the backend and redirect them to clients.
     */
    LPointer *pointer() const;

    /**
     * @brief Access to keyboard events.
     *
     * Access to the LKeyboard instance used to receive keyboard events from the backend and redirect them to clients.
     */
    LKeyboard *keyboard() const;

    /**
     * @brief Access to touch events.
     *
     * Access to the LTouch instance used to receive touch events from the backend and redirect them to clients.
     */
    LTouch *touch() const;

    /**
     * @brief Access to the drag & drop session manager.
     *
     * Access to the LDNDManager instance used to handle drag & drop sessions.
     */
    LDNDManager *dndManager() const;

    /**
     * @brief Access to the clipboard.
     *
     * Access to the clipboard (data source) assigned by the last client.\n
     * @returns `nullptr` if the clipboard has not been assigned.
     */
    LDataSource *dataSelection() const;

    ///@}

    /**
     * @name Interactive toplevel Movement
     *
     * These utility methods simplify the management of interactive toplevel moving sessions.
     *
     * @note Using these methods is optional.
     *
     * @see LToplevelRole::startMoveRequest()
     */

    ///@{

    /**
     * @brief Initiate an interactive toplevel moving session.
     *
     * This method initiates an interactive moving session for a toplevel surface.\n
     * You can confine the Toplevel's placement within a rectangle by specifying values for L, T, R, and B.\n
     * If you don't wish to restrict any edges, set their values to LPointer::EdgeDisabled.
     *
     * To update the Toplevel's position, use the updateMovingToplevelPos() method.
     * Once the position change is complete, use the stopMovingToplevel() method to conclude the session.
     *
     * @note The session will automatically cease if the toplevel is destroyed.
     *
     * @see See an example of its use in LToplevelRole::startMoveRequest().
     *
     * @param toplevel The toplevel whose size will change.
     * @param pointerPos Current pointer position.
     * @param L Restriction for the left edge.
     * @param T Restriction for the top edge.
     * @param R Restriction for the right edge.
     * @param B Restriction for the bottom edge.
     */
    void startMovingToplevel(LToplevelRole *toplevel,
                             const LPoint &pointerPos,
                             Int32 L = LToplevelRole::EdgeDisabled, Int32 T = LToplevelRole::EdgeDisabled,
                             Int32 R = LToplevelRole::EdgeDisabled, Int32 B = LToplevelRole::EdgeDisabled);

    /**
     * @brief Update the position of a toplevel during an interactive moving session.
     *
     * Call this method when the pointer position changes.
     *
     * @see See an example of its usage in the default implementation of pointerPosChangeEvent().
     *
     * @param pointerPos The current pointer position.
     */
    void updateMovingToplevelPos(const LPoint &pointerPos);

    /**
     * @brief Conclude an interactive moving session.
     *
     * Use this method to conclude a moving toplevel session, for example, when releasing the left pointer button.
     *
     * @see See an example of its usage in the default implementation of pointerButtonEvent().
     */
    void stopMovingToplevel();

    /**
     * @brief Get the toplevel surface involved in an interactive moving session.
     *
     * @return A pointer to the toplevel surface, or `nullptr` if there is no ongoing interactive moving session.
     */
    LToplevelRole *movingToplevel() const;

    /**
     * @brief Retrieve the initial position of a toplevel during an interactive moving session.
     *
     * This method provides the initial position of a toplevel surface when an interactive moving session begins.
     */
    const LPoint &movingToplevelInitPos() const;

    /**
     * @brief Retrieve the initial pointer position during a toplevel interactive moving session.
     *
     * This method provides the initial pointer position when an interactive moving session of a toplevel surface starts.
     */
    const LPoint &movingToplevelInitPointerPos() const;

    ///@}

/// @name Session
/// @{
    /**
     * @brief Switch session.
     *
     * This method allows you to switch to another session (TTY). Louvre also allows switching sessions
     * using the shortcuts ```Ctrl + Alt + [F1, F2, ..., F10]```.
     *
     * @param tty TTY number.
     */
    void setTTY(UInt32 tty);

    /**
     * @brief Open a device.
     *
     * Opens a device on the seat, returning its ID and storing its file descriptor in **fd**.\n
     * The DRM graphic backend and the Libinput input backend use this method to open GPUs and input devices respectively.
     *
     * @param path Location of the device (E.g. "/dev/dri/card0")
     * @param fd Stores the file descriptor.
     * @returns The ID of the device or -1 in case of an error.
     */
    Int32 openDevice(const char *path, Int32 *fd);

    /**
     * @brief Close a device.
     *
     * This method is used by the input and graphic backends to close devices.
     *
     * @param id The id of the device returned by openDevice().
     * @returns 0 if the device is closed successfully and -1 in case of error.
     */
    Int32 closeDevice(Int32 id);

    /**
     * @brief Libseat handle.
     *
     * [struct libseat](https://github.com/kennylevinsen/seatd/blob/master/include/libseat.h) handle.
     */
    libseat *libseatHandle() const;

    /**
     * @brief Check the session state.
     *
     * The session is considered disabled if the user is engaged in another active session (TTY).
     *
     * @return `true` if the seat is active, `false` otherwise.
     */
    bool enabled() const;

    /**
     * @brief Retrieve the topmost popup role.
     *
     * This method returns a pointer to the topmost popup.
     *
     * @return A pointer to the topmost LPopupRole or `nullptr` if there is no popup.
     */
    LPopupRole *topmostPopup() const;

/// @}

/// @name Virtual Methods
/// @{
    /**
     * @brief Seat initialization.
     *
     * Reimplement this virtual method if you want to be notified when the seat is initialized.\n
     * The seat is initialized during the compositor's initialization process.
     *
     * #### Default implementation
     * @snippet LSeatDefault.cpp initialized
     */
    virtual void initialized();

    /**
     * @brief Request to set the clipboard.
     *
     * Reimplement this virtual method if you want to control which clients can set the clipboard.\n
     * The default implementation allows clients to set the clipboard only if one of their surfaces has pointer or keyboard focus.\n
     *
     * Returning `true` grants the client permission to set the clipboard and `false` prohibits it.\n
     *
     * @param device Data device that makes the request.
     *
     * #### Default Implementation
     * @snippet LSeatDefault.cpp setSelectionRequest
     */
    virtual bool setSelectionRequest(LDataDevice *device);

    /**
     * @brief Native input backend events
     *
     * Reimplement this virtual method if you want to access all events generated by the input backend.\n
     *
     * @param event Opaque handle to the native backend event. In the Libinput backend it corresponds to a [libinput_event](https://wayland.freedesktop.org/libinput/doc/latest/api/structlibinput__event.html) struct 
     * and in the X11 backend to a [XEvent](https://www.x.org/releases/X11R7.6/doc/libX11/specs/libX11/libX11.html) struct.
     *
     * #### Default implementation
     * @snippet LSeatDefault.cpp backendNativeEvent
     */
    virtual void nativeInputEvent(void *event);

    /**
     * @brief Notify a change in the enabled() property
     *
     * Reimplement this virtual method if you want to be notified when the seat is enabled or disabled.\n
     * The seat is enabled when the user is in the session (TTY) in which the compositor is running,
     * and disabled when switching to a different session.
     *
     * @see setTTY()
     *
     * #### Default implementation
     * @snippet LSeatDefault.cpp enabledChanged
     */
    virtual void enabledChanged();

    /**
     * @brief New available output.
     *
     * This event is invoked by the graphic backend when a new output is available, for example when connecting an external monitor through an HDMI port.\n
     * You can reimplement this method to be notified when a new output is available.\n
     * The default implementation initializes the new output and positions it at the end (right) of the already initialized outputs.
     *
     * ### Default Implementation
     * @snippet LSeatDefault.cpp outputPlugged
     */
    virtual void outputPlugged(LOutput *output);

    /**
     * @brief Disconnected output.
     *
     * This event is invoked by the graphic backend when an output is no longer available, for example when an
     * external monitor connected to an HDMI port is disconnected.\n
     * You can override this method to be notified when an output is no longer available.\n
     *
     * The default implementation removes the output from the compositor if it is initialized
     * and re-arranges the ones already initialized.
     *
     * @note Louvre automatically calls LCompositor::removeOutput() after this event, so calling it directly is not mandatory.
     *
     * #### Default Implementation
     * @snippet LSeatDefault.cpp outputUnplugged
     */
    virtual void outputUnplugged(LOutput *output);

    /**
     * @brief New available input device.
     *
     * Override this method to be notified when a new input device is avaliable.\n
     * The default implementation aligns the input capabilities of the compositor with those of the input backend.
     *
     * #### Default Implementation
     * @snippet LSeatDefault.cpp inputDevicePlugged
     */
    virtual void inputDevicePlugged(LInputDevice *device);

    /**
     * @brief Disconnected input device.
     *
     * Override this method to be notified when an input device is no longer avaliable.\n
     * The default implementation aligns the input capabilities of the compositor with those of the input backend.
     *
     * #### Default Implementation
     * @snippet LSeatDefault.cpp inputDeviceUnplugged
     */
    virtual void inputDeviceUnplugged(LInputDevice *device);

/// @}

    LPRIVATE_IMP_UNIQUE(LSeat)
};

#endif // LSEAT_H
