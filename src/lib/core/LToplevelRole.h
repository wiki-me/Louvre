#ifndef LTOPLEVELROLE_H
#define LTOPLEVELROLE_H
#undef None

#include <LBaseSurfaceRole.h>
#include <LSize.h>
#include <LRect.h>
#include <limits>

/**
 * @brief Toplevel role for surfaces
 *
 * The LToplevelRole class is a role for surfaces used by clients to display application windows that have
 * a title and buttons to close, minimize and maximize.\n
 *
 * <center><img height="250px" src="https://lh3.googleusercontent.com/PvNaxLgkjaPryJ8W_P3bDOccQu1m-zNEcI7aH_R8WggzylV5LQZtuzLTUSImThDI8IVsAI9DERF4cwvSqPAEAyjsihHuPCQlZAbvu33iMC2iXvpTswZC3RBNDyKm1YEWDnnKeCn2Qw=w2400"></center>
 *
 * The toplevel role is part of the [XDG Shell](https://wayland.app/protocols/xdg-shell#xdg_toplevel) protocol.
 * The Wayland protocol also provides its own toplevel role but it is considered obsolete and therefore not included in the library.
 */
class Louvre::LToplevelRole : public LBaseSurfaceRole
{
public:

    struct Params;

    /**
     * @brief LToplevelRole class constructor.
     *
     * @param params Internal library parameters provided in the virtual LCompositor::createToplevelRoleRequest() constructor.
     */
    LToplevelRole(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LToplevelRole)
    /// @endcond

    /**
     * @brief LToplevelRole class destructor.
     *
     * Invoked after LCompositor::destroyToplevelRoleRequest().
     */
    virtual ~LToplevelRole();

    /**
     * @brief Resizing border/corner.
     */
    enum ResizeEdge : UInt32
    {
        /// No edge
        NoEdge = 0,

        /// Top edge
        Top = 1,

        /// Bottom edge
        Bottom = 2,

        /// Left edge
        Left = 4,

        /// Top left corner
        TopLeft = 5,

        /// Bottom left corner
        BottomLeft = 6,

        /// Right edge
        Right = 8,

        /// Top right corner
        TopRight = 9,

        /// Bottom right corner
        BottomRight = 10
    };

    /**
     * @brief Edge constraint when resizing a Toplevel
     */
    enum ResizeEdgeSize : Int32
    {
        /// Disables the constraint on the specified edge.
        EdgeDisabled = std::numeric_limits<Int32>::min()
    };

    /**
     * @brief Toplevel states bitfield
     */
    typedef UInt32 States;

    /**
     * @brief Flags indicating the possible states of a Toplevel.
     */
    enum State : States
    {
        /// No state
        NoState     = 0,

        /// Maximized
        Maximized   = 1,

        /// Fullscreen mode
        Fullscreen  = 2,

        /// In interactive resizing
        Resizing    = 4,

        /// Activated (its decorations stand out from others)
        Activated   = 8,

        /// Tiled left (since 2)
        TiledLeft   = 16,

        /// Tiled right (since 2)
        TiledRight  = 32,

        /// Tiled top (since 2)
        TiledTop    = 64,

        /// Tiled bottom (since 2)
        TiledBottom = 128
    };

    /// Decoration mode
    enum DecorationMode : UInt32
    {
        /// The client has no preferred mode
        NoPreferredMode = 0,

        /// Decorations are drawn by the client
        ClientSide = 1,

        /// Decorations are drawn by the compositor
        ServerSide = 2
    };

    /**
     * @brief Configures the Toplevel.
     *
     * Requests to change the states of the toplevel while keeping its pending size.\n
     *
     * @param stateFlags Union of toplevel states defined in #States.
     */
    void configure(States stateFlags);

    /**
     * @brief Configures the Toplevel.
     *
     * Requests to change the size and states of the Toplevel.\n
     * The size refers to its window geometry (ignoring its decoration).
     *
     * @param size Requested size. Passing (0,0) allows the client to decide the size.
     * @param stateFlags Union of toplevel states defined in #States.
     */
    void configure(const LSize &size, States stateFlags);

    /**
     * @brief Configures the Toplevel.
     *
     * Requests to change the size and states of the Toplevel.\n
     * The size refers to its window geometry (ignoring its decoration).
     *
     * @param width Suggested width. Passing (0,0) allows the client to decide the size.
     * @param height Suggested height. Passing (0,0) allows the client to decide the size.
     * @param stateFlags Union of toplevel states defined in #States.
     */
    void configure(Int32 width, Int32 height, States stateFlags);

    /**
     * @brief Closes the Toplevel.
     *
     * Requests to close the toplevel (equivalent to pressing the close button on the window).
     */
    void close() const;

    /**
     * @brief Window geometry in surface coordinates
     *
     * <center><img height="300px" src="https://lh3.googleusercontent.com/pw/AIL4fc_le5DeTa6b-yBnChX6YPbkr12gAp38ghVyvsv4SjHCd2L4fTL8agYls0AcGlBeplJyc0FNQCIeb6sR4WbSUyAHM4_LrKLNjhZ0SniRdaSUsjS9IGQ=w2400"></center>
     *
     * The window geometry is a toplevel rectangle that excludes its decorations (typically shadows).     *
     */
    const LRect &windowGeometry() const;

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
     * @param movePointPos Current move point position (cursor position, touch point position, etc).
     * @param L Restriction for the left edge.
     * @param T Restriction for the top edge.
     * @param R Restriction for the right edge.
     * @param B Restriction for the bottom edge.
     */
    bool startMoveSession(const LEvent &triggerigEvent,
                          const LPoint &movePointPos,
                          Int32 L = LToplevelRole::EdgeDisabled, Int32 T = LToplevelRole::EdgeDisabled,
                          Int32 R = LToplevelRole::EdgeDisabled, Int32 B = LToplevelRole::EdgeDisabled);

    LToplevelMoveSession *moveSession() const;

    ///@}

    /**
     * @name Interactive Toplevel Resizing
     *
     * These utility methods simplify the management of interactive toplevel resizing sessions.
     *
     * @note Using these methods is optional.
     *
     * @see LToplevelRole::startResizeRequest()
     * @see LToplevelRole::geometryChanged()
     */

    ///@{

    /**
     * @brief Start an interactive toplevel resizing session.
     *
     * This method starts an interactive resizing session on a toplevel surface from one of its edges or corners.\n
     * You can restrict the space in which the surface expands by defining a rectangle given by the L, T, R, and B values.\n
     * If you do not want to restrict an edge, assign its value to LPointer::EdgeDisabled.
     *
     * To update the position and size of the Toplevel, call updateResizingToplevelSize() when the pointer moves and
     * updateResizingToplevelPos() when the toplevel size changes.\n
     * Once finished, call stopResizingToplevel() to end the session.
     *
     * @note The session will automatically cease if the toplevel is destroyed.
     *
     * @see See an example of its use in LToplevelRole::startResizeRequest().
     *
     * @param toplevel Toplevel that will change size.
     * @param edge Edge or corner from which the resizing will be performed.
     * @param pointerPos Current pointer position.
     * @param minSize Minimum toplevel size.
     * @param L Restriction of the left edge.
     * @param T Restriction of the top edge.
     * @param R Restriction of the right edge.
     * @param B Restriction of the bottom edge.
     */
    bool startResizeSession(const LEvent &triggeringEvent,
                           LToplevelRole::ResizeEdge edge,
                           const LPoint &resizePointPos,
                           const LSize &minSize = LSize(0, 0),
                           Int32 L = EdgeDisabled, Int32 T = EdgeDisabled,
                           Int32 R = EdgeDisabled, Int32 B = EdgeDisabled);


    LToplevelResizeSession *resizeSession() const;

    /**
     * @brief Size during a resizing session
     *
     * Utility method to calculate the toplevel size during an interactive resizing session.\n
     *
     * @param resizePointDelta Initial resize point position minus the current one
     * @param initialSize Initial toplevel position
     * @param edge Edge or corner used in the resizing
     *
     * @returns The toplevel size given the current parameters
     */
    LSize calculateResizeSize(const LPoint &resizePointDelta, const LSize &initialSize, ResizeEdge edge);

    /**
     * @brief Get the application ID associated with the toplevel window.
     *
     * @return A string containing the application ID (e.g., "com.cuarzosoftware.Desk"), can be `nullptr`.
     */
    const char *appId() const;

    /**
     * @brief Get the window title of the toplevel.
     *
     * @return A string representing the window title, or `nullptr`.
     */
    const char *title() const;

    /**
     * @brief Get the minimum size of the window geometry in surface coordinates.
     *
     * If one of the axis is 0, it means it has no minimum size.
     *
     * @return The minimum size as a Louvre::LSize object.
     */
    const LSize &minSize() const;

    /**
     * @brief Get the maximum size of the window geometry in surface coordinates.
     *
     * If one of the axis is 0, it means it has no maximum size.
     *
     * @return The maximum size as a Louvre::LSize object.
     */
    const LSize &maxSize() const;

    /**
     * @brief Check if the toplevel window is being resized.
     *
     * @return `true` if the toplevel is being resized; otherwise, `false`.
     */
    bool resizing() const;

    /**
     * @brief Check if the toplevel window is maximized.
     *
     * @return `true` if the toplevel is maximized; otherwise, `false`.
     */
    bool maximized() const;

    /**
     * @brief Check if the toplevel window is in fullscreen mode.
     *
     * @return `true` if the toplevel is in fullscreen mode; otherwise, `false`.
     */
    bool fullscreen() const;

    /**
     * @brief Check if the toplevel window is currently active.
     *
     * @return `true` if the toplevel is active; otherwise, `false`.
     */
    bool activated() const;

    /**
     * @brief Get the flags representing the current states of the toplevel window.
     *
     * @return The states of the toplevel window as a #States bitfield.
     */
    States currentState() const;
    States pendingState() const;

    /**
     * @brief Request the client to change its decoration mode.
     *
     * The client may choose to ignore the request. Must be followed by a configure() event.
     *
     * @note LToplevelRole::NoPreferredMode is not a valid decoration mode.
     *
     * @param mode The desired decoration mode for the toplevel window.
     */
    void setDecorationMode(DecorationMode mode);

    /**
     * @brief Get the current decoration mode of the toplevel window.
     *
     * @return The current decoration mode.
     */
    DecorationMode decorationMode() const;

    /**
     * @brief Get the preferred decoration mode set by the client.
     *
     * @return The preferred decoration mode value.
     */
    DecorationMode preferredDecorationMode() const;

    /**
     *  @brief [xdg_toplevel](https://wayland.app/protocols/xdg-shell#xdg_toplevel) resource from the [XDG Shell](https://wayland.app/protocols/xdg-shell) protocol.
     */
    Protocols::XdgShell::RXdgToplevel *xdgToplevelResource() const;

    /**
     *  @brief [xdg_surface](https://wayland.app/protocols/xdg-shell#xdg_surface) resource from the [XDG Shell](https://wayland.app/protocols/xdg-shell) protocol.
     */
    Protocols::XdgShell::RXdgSurface *xdgSurfaceResource() const;

/// @name Virtual Methods
/// @{
    /**
     * @brief Position of the surface according to the role.
     *
     * The default implementation of rolePos() positions the toplevel at the position assigned by the compositor
     * with LSurface::setPos() minus the (x, y) coords of its window geometry.
     *
     * Reimplement this virtual method if you wish to define your own logic for positioning the Toplevel.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp rolePos
     */
    virtual const LPoint &rolePos() const override;

    /**
     * @brief Request to start an interactive move session
     *
     * Reimplement this virtual method if you wish to be notified when the client wishes to start an interactive move session.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp startMoveRequest
     */
    virtual void startMoveRequest(const LEvent &triggeringEvent);

    /**
     * @brief Request to start an interactive resize session
     *
     * Reimplement this virtual method if you want to be notified when the client wants to start an interactive resize session.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp startResizeRequest
     */
    virtual void startResizeRequest(const LEvent &triggeringEvent, ResizeEdge edge);

    /**
     * @brief Resizing state change
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its resizing state.\n
     * You can know if the toplevel is being resized with resizing().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp resizingChanged
     */
    virtual void resizingChanged();

    /**
     * @brief Request for configuration
     *
     * Reimplement this virtual method if you want to be notified when the client wants the compositor to configure the Toplevel.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp configureRequest
     */
    virtual void configureRequest();

    /**
     * @brief Request to maximize
     *
     * Reimplement this virtual method if you want to be notified when the client wants to maximize the Toplevel.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp setMaximizedRequest
     */
    virtual void setMaximizedRequest();

    /**
     * @brief Request to unmaximize
     *
     * Reimplement this virtual method if you want to be notified when the client wants to unmaximize the Toplevel.
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp unsetMaximizedRequest
     */
    virtual void unsetMaximizedRequest();

    /**
     * @brief Maximized state change
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its maximized state.\n
     * You can know if the toplevel is maximized with maximized().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp maximizedChanged
     */
    virtual void maximizedChanged();

    /**
     * @brief Request to minimize
     *
     * Reimplement this virtual method if you want to be notified when the client wants to minimize the Toplevel.
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp setMinimizedRequest
     */
    virtual void setMinimizedRequest();

    /**
     * @brief Request to set fullscreen mode
     *
     * Reimplement this virtual method if you want to be notified when the client wants to show the toplevel in fullscreen mode.
     *
     * @param destOutput Output on which the toplevel should be shown. If it is `nullptr` the compositor should choose the output.
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp setFullscreenRequest
     */
    virtual void setFullscreenRequest(LOutput *destOutput);

    /**
     * @brief Request to unset fullscreen mode
     *
     * Reimplement this virtual method if you want to be notified when the client wants to deactivate fullscreen mode.
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp unsetFullscreenRequest
     */
    virtual void unsetFullscreenRequest();

    /**
     * @brief Change in fullscreen mode state
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its fullscreen mode state.\n
     * You can know if the toplevel is in fullscreen mode with fullscreen().
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp fullscreenChanged
     */
    virtual void fullscreenChanged();

    /**
     * @brief Request to show a menu
     *
     * Reimplement this virtual method if you want to be notified when the client requests the compositor to show a menu
     * containing options to minimize, maximize and change to fullscreen mode.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp showWindowMenuRequest
     */
    virtual void showWindowMenuRequest(Int32 x, Int32 y);

    /**
     * @brief Change of active state
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its active state.\n
     * You can check if the toplevel is active with activated().
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp activatedChanged
     */
    virtual void activatedChanged();

    /**
     * @brief Notification of State Changes
     *
     * Reimplement this virtual method if you wish to receive notifications when the Toplevel's states flags change.
     *
     * You can access the Toplevel's state flags using the states() property.
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp statesChanged
     */
    virtual void statesChanged();

    /**
     * @brief Change of maximum size
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its maximum size.\n
     * You can access the maximum size with maxSize().
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp maxSizeChanged
     */
    virtual void maxSizeChanged();

    /**
     * @brief Change of minimum size
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its minimum size.\n
     * You can access the minimum size with minSize().
     *
     * #### Default Implementation
     * @snippet LToplevelRoleDefault.cpp minSizeChanged
     */
    virtual void minSizeChanged();

    /**
     * @brief Title change
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its title.
     * You can access the Toplevel's title with title().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp titleChanged
     */
    virtual void titleChanged();

    /**
     * @brief Application ID change
     *
     * Reimplement this virtual method if you want to be notified when the client changes its application ID.
     * You can access the application ID with appId().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp appIdChanged
     */
    virtual void appIdChanged();

    /**
     * @brief Window geometry change
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its window geometry.
     * You can access the Toplevel's window geometry with windowGeometry().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp geometryChanged
     */
    virtual void geometryChanged();

    /**
     * @brief Decoration mode change
     *
     * Reimplement this virtual method if you want to be notified when the toplevel changes its decoration mode.
     * You can access the Toplevel's decoration mode with decorationMode().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp decorationModeChanged
     */
    virtual void decorationModeChanged();

    /**
     * Reimplement this virtual method if you want to be notified when the toplevel changes its preferred decoration mode.
     * You can access the Toplevel's preferred decoration mode with preferredDecorationMode().
     *
     * #### Default implementation
     * @snippet LToplevelRoleDefault.cpp preferredDecorationModeChanged
     */
    virtual void preferredDecorationModeChanged();
/// @}

    LPRIVATE_IMP_UNIQUE(LToplevelRole)

    /// @cond OMIT
    void handleSurfaceCommit(Protocols::Wayland::RSurface::CommitOrigin origin) override;
    /// @endcond
};

#endif // LTOPLEVELROLE_H
