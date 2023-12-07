#ifndef GPRESENTATION_H
#define GPRESENTATION_H

#include <LResource.h>

class Louvre::Protocols::PresentationTime::GPresentation : public LResource
{
public:
    GPresentation(wl_client *client,
                    const wl_interface *interface,
                    Int32 version,
                    UInt32 id,
                    const void *implementation,
                    wl_resource_destroy_func_t destroy);
    LCLASS_NO_COPY(GPresentation)
    ~GPresentation();

    // Since 1
    bool clockId(UInt32 clockId);

    LPRIVATE_IMP_UNIQUE(GPresentation)
};

#endif // GPRESENTATION_H
