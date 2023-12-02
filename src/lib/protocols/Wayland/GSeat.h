#ifndef GSEAT_H
#define GSEAT_H

#include <LResource.h>

class Louvre::Protocols::Wayland::GSeat : public LResource
{
public:
    GSeat(wl_client *client,
          const wl_interface *interface,
          Int32 version,
          UInt32 id,
          const void *implementation,
          wl_resource_destroy_func_t destroy);
    LCLASS_NO_COPY(GSeat)
    ~GSeat();

    LEvent *findSerialEventMatch(UInt32 serial) const;

    RKeyboard *keyboardResource() const;
    RPointer *pointerResource() const;
    RTouch *touchResource() const;
    RDataDevice *dataDeviceResource() const;

    // Since 1
    bool capabilities(UInt32 capabilities);

    // Since 2
    bool name(const char *name);

    LPRIVATE_IMP_UNIQUE(GSeat)
};

#endif // GSEAT_H
