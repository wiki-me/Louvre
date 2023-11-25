#ifndef LDNDMANAGER_H
#define LDNDMANAGER_H

#include <LObject.h>

/**
 * @brief Class for handling drag & drop sessions
 *
 * The LDNDManager class provides control over drag & drop sessions and its unique instance can be accessed with LSeat::dndManager().\n
 * It has virtual methods that notify when a client wants to start or cancels a drag & drop session, methods for
 * "drop" or cancel a data offering, and more.
 */
class Louvre::LDNDManager : public LObject
{
public:
    struct Params;

    /**
     * @brief Constructor of the LDNDManager class.
     *
     * There is a single instance of LDNDManager, which can be accessed from LSeat::dndManager().
     *
     * @param params Internal library parameters passed in the LCompositor::createDNDManagerRequest() virtual constructor.
     */
    LDNDManager(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LDNDManager)
    /// @endcond

    /**
     * @brief Destructor of the LDNDManager class.
     *
     * Invoked internally by the library after LCompositor::destroyDNDManagerRequest() is called.
     */
    virtual ~LDNDManager();

    /**
     * @brief Action flags for drag & drop sessions.
     *
     * Clients who start drag & drop sessions or receive a data offer notify which actions they support.\n
     * For example, when dragging a file from a file manager window to another, the desired action might be to
     * move or copy the file.\n
     * The library by default performs a match of actions supported by the source and destination client, giving preference
     * to the first one listed in the enum, except for LDNDManager::NoAction.\n
     * You can also select a different preferred action using the LDNDManager::setPreferredAction() method as exemplified in the
     * default implementation of LKeyboard::keyEvent().
     */
    enum Action : UInt32
    {
        /// No preferred action
        NoAction = 0,

        /// The preferred action is to copy
        Copy = 1,

        /// The preferred action is to move
        Move = 2,

        /// The destination client asks the source client the preferred action
        Ask = 4
    };

    /**
     * @brief Drag & drop session icon.
     *
     * LDNDIconRole of the surface used as drag & drop icon.
     *
     * @note Not all drag & drop sessions use an icon.
     *
     * @returns `nullptr` if there is no session going on, or if the source client did not assign an icon.
     */
    LDNDIconRole *icon() const;

    /**
     * @brief Surface that originates the drag & drop session.
     *
     * Surface from which the drag & drop session originates.
     *
     * @returns `nullptr` if there is no session going on.
     */
    LSurface *origin() const;

    /**
     * @brief Focused surface.
     *
     * Surface to which the data offer is being presented to.\n
     * It typically is the surface located under the cursor.
     *
     * @returns `nullptr` if there is no session going on or if no surface has focus.
     */
    LSurface *focus() const;

    /**
     * @brief Data source.
     *
     * Data source of the session, generated by the source client.
     *
     * @returns `nullptr` if there is no session going on.
     */
    LDataSource *source() const;

    /**
     * @brief Data device resource that started the drag & drop session.
     *
     * @returns `nullptr` if there is no session going on.
     */
    Protocols::Wayland::RDataDevice *srcDataDevice() const;

    /**
     * @brief Get the destination client for the current data offer.
     *
     * This method returns the client to whom the current data offer is being made.
     *
     * @returns A pointer to the destination client, or `nullptr` if there is no current destination client.
     */
    LClient *dstClient() const;

    /**
     * @brief Check if a drag & drop session is currently in progress.
     *
     * @return `true` if there is an ongoing drag & drop session, `false` otherwise.
     */
    bool dragging() const;

    /**
     * @brief Cancels the session.
     *
     * Cancels the current drag & drop session.\n
     * If there is no session going on, calling this method is a no-op.
     */
    void cancel();

    /**
     * @brief Drop the data offer.
     *
     * Drop the data offer on the surface with active focus and ends the session.\n
     * The destination client then exchanges the information with the source client, using the specified action.\n
     * If there is no focused surface, the session is cancelled.
     *
     * The library invokes this method when the left mouse button is released (check the default implementation of LPointer::pointerButtonEvent()).
     */
    void drop();

    /**
     * @brief Preferred action of the compositor.
     *
     * Returns the preferred actions of the compositor set whith setPreferredAction().
     */
    Action preferredAction() const;

    /**
     * @brief Assigns the preferred action.
     *
     * Assigns the compositor's preferred action for the session.
     * The library by default assigns the LDNDManager::Move action when holding down the `Shift`
     * key and the LDNDManager::Copy action when holding down the `Ctrl` key
     * (check the default implementation of LKeyboard::keyEvent()).\n
     * If the specified action is not supported by the source and destination client calling this method is a no-op.
     */
    void setPreferredAction(Action action);

    /**
     * @brief Request to start a drag & drop session
     *
     * Reimplement this virtual method if you want to be notified when a client wants to start a drag & drop session.\n
     * This method is invoked only when there is no session in progress.\n
     * The default implementation validates that the client that originates the request has a surface with keyboard or pointer focus. If
     * neither condition is met the session is cancelled.
     *
     * #### Default implementation
     * @snippet LDNDManagerDefault.cpp startDragRequest
     */
    virtual void startDragRequest();

    /**
     * @brief Notifies the cancellation of a session
     *
     * Reimplement this virtual method if you want to be notified when a drag & drop session is cancelled.\n
     * The default implementation repaints outputs where the drag & drop icon was visible.
     *
     * #### Default implementation
     * @snippet LDNDManagerDefault.cpp cancelled
     */
    virtual void cancelled();

    LPRIVATE_IMP_UNIQUE(LDNDManager)
};

#endif // LDNDMANAGER_H
