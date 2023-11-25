#include <private/LCompositorPrivate.h>
#include <LInputDevice.h>

using namespace Louvre;

LInputDevice::LInputDevice(const Interface &interface, void *backendData) :
    m_interface(interface),
    m_backendData(backendData)
{}

LInputDevice::~LInputDevice() {}

const char *LInputDevice::name() const
{
    return m_interface.name(this);
}

UInt32 LInputDevice::productId() const
{
    return m_interface.productId(this);
}

UInt32 LInputDevice::vendorId() const
{
    return m_interface.vendorId(this);
}

void *LInputDevice::backendData() const
{
    return m_backendData;
}
