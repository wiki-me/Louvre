#include <protocols/PresentationTime/private/GPresentationPrivate.h>
#include <protocols/PresentationTime/presentation-time.h>
#include <private/LClientPrivate.h>

using namespace Louvre::Protocols::PresentationTime;

GPresentation::GPresentation
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
    LPRIVATE_INIT_UNIQUE(GPresentation)
{
    this->client()->imp()->presentationTimeGlobals.push_back(this);
    imp()->clientLink = std::prev(this->client()->imp()->presentationTimeGlobals.end());
    clockId(CLOCK_MONOTONIC);
}

GPresentation::~GPresentation()
{
    client()->imp()->presentationTimeGlobals.erase(imp()->clientLink);
}

bool GPresentation::clockId(UInt32 clockId)
{
    wp_presentation_send_clock_id(resource(), clockId);
    return true;
}
