#include <protocols/Wayland/private/GDataDeviceManagerPrivate.h>
#include <private/LClientPrivate.h>

using namespace Louvre::Protocols::Wayland;

GDataDeviceManager::GDataDeviceManager
(
    LClient *client,
    const wl_interface *interface,
    Int32 version,
    UInt32 id,
    const void *implementation,
    wl_resource_destroy_func_t destroy
)
    :LResource
    (
        client,
        interface,
        version,
        id,
        implementation,
        destroy
    ),
    LPRIVATE_INIT_UNIQUE(GDataDeviceManager)
{
    client->imp()->dataDeviceManagerGlobals.push_back(this);
    imp()->clientLink = std::prev(client->imp()->dataDeviceManagerGlobals.end());
}

GDataDeviceManager::~GDataDeviceManager()
{
    client()->imp()->dataDeviceManagerGlobals.erase(imp()->clientLink);
}
