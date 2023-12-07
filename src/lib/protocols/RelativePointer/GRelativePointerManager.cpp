#include <protocols/RelativePointer/private/GRelativePointerManagerPrivate.h>
#include <protocols/RelativePointer/relative-pointer-unstable-v1.h>
#include <private/LClientPrivate.h>

using namespace Louvre::Protocols::RelativePointer;

GRelativePointerManager::GRelativePointerManager
    (
        wl_client *client,
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
    LPRIVATE_INIT_UNIQUE(GRelativePointerManager)
{
    this->client()->imp()->relativePointerManagerGlobals.push_back(this);
    imp()->clientLink = std::prev(this->client()->imp()->relativePointerManagerGlobals.end());
}

GRelativePointerManager::~GRelativePointerManager()
{
    client()->imp()->relativePointerManagerGlobals.erase(imp()->clientLink);
}

