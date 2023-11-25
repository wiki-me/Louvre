#ifndef RXDGPOSITIONER_H
#define RXDGPOSITIONER_H

#include <LResource.h>

class Louvre::Protocols::XdgShell::RXdgPositioner : public LResource
{
public:
    RXdgPositioner(GXdgWmBase *gXdgWmBase, UInt32 id);
    LCLASS_NO_COPY(RXdgPositioner)
    ~RXdgPositioner();

    const LPositioner &positioner() const;
    bool isValid();

    LPRIVATE_IMP_UNIQUE(RXdgPositioner)
};

#endif // RXDGPOSITIONER_H
