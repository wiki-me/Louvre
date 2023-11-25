#ifndef GCOMPOSITOR_H
#define GCOMPOSITOR_H

#include <LResource.h>

class Louvre::Protocols::Wayland::GCompositor : public LResource
{
public:
    GCompositor(LClient *client,
                const wl_interface *interface,
                Int32 version,
                UInt32 id,
                const void *implementation,
                wl_resource_destroy_func_t destroy);
    LCLASS_NO_COPY(GCompositor)
    ~GCompositor();
    LPRIVATE_IMP_UNIQUE(GCompositor)
};

#endif // GCOMPOSITOR_H
