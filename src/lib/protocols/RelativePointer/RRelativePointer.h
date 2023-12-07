#ifndef RRELATIVEPOINTER_H
#define RRELATIVEPOINTER_H

#include <LResource.h>

class Louvre::Protocols::RelativePointer::RRelativePointer : public LResource
{
public:
    RRelativePointer(Protocols::Wayland::RPointer *rPointer, Int32 id, UInt32 version);
    LCLASS_NO_COPY(RRelativePointer)
    ~RRelativePointer();

    Protocols::Wayland::RPointer *pointerResource() const;

    // Since 1
    bool relative_motion(const LPointerMoveEvent &event);

    LPRIVATE_IMP_UNIQUE(RRelativePointer);
};

#endif // RRELATIVEPOINTER_H
