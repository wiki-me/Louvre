#ifndef LINPUTDEVICE_H
#define LINPUTDEVICE_H

#include <LObject.h>

class Louvre::LInputDevice : public LObject
{
public:
    struct Interface
    {
        const char * (*name)(const LInputDevice *);
        UInt32 (*vendorId)(const LInputDevice *);
        UInt32 (*productId)(const LInputDevice *);
    };

    LInputDevice(const Interface &interface, void *backendData);

    /// @cond OMIT
    LCLASS_NO_COPY(LInputDevice)
    /// @endcond

    ~LInputDevice();

    const char *name() const;
    UInt32 productId() const;
    UInt32 vendorId() const;
    void *backendData() const;
private:
    friend class LInputBackend;
    Interface m_interface;
    void *m_backendData;
};

#endif // LINPUTDEVICE_H
