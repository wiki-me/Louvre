#include <private/LCompositorPrivate.h>
#include <LInputEvent.h>
#include <LTime.h>

using namespace Louvre;

LInputEvent::LInputEvent(Type type) :
    m_device(&compositor()->imp()->fakeDevice),
    m_time(LTime::ms()),
    m_type(type)
{}

LInputEvent::~LInputEvent() {}
