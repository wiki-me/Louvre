#ifndef LINPUTBACKEND
#define LINPUTBACKEND

#include <LNamespaces.h>
#include <LPointer.h>

class Louvre::LInputBackend
{
public:

    // Backend Identifier
    static UInt32 id();

    // Initialize the backend
    static bool initialize();

    // Get capabilities (pointer, keyboard, touch)
    static UInt32 getCapabilities();

    // Get context handle
    static void *getContextHandle();

    // Suspend events when switching TTY
    static void suspend();

    // Force processing of pending events
    static void forceUpdate();

    // Resume events upon returning to the TTY
    static void resume();

    // Deinitialize the backend
    static void uninitialize();

    // Devices
    static std::list<LInputDevice*>*getDevices();

    // Events
    static Int32 processInput(int, unsigned int, void *userData);
    static LInputDevice *inputEventGetDevice(const LInputEvent *event);

    static UInt32 pointerButtonEventGetButton(const LPointerButtonEvent *event);
    static UInt32 pointerButtonEventGetButtonState(const LPointerButtonEvent *event);

    static Float32 pointerAxisEventGetAxisX(const LPointerAxisEvent *event);
    static Float32 pointerAxisEventGetAxisY(const LPointerAxisEvent *event);
    static Float32 pointerAxisEventGetDiscreteX(const LPointerAxisEvent *event);
    static Float32 pointerAxisEventGetDiscreteY(const LPointerAxisEvent *event);
    static UInt32 pointerAxisEventGetSource(const LPointerAxisEvent *event);
};

#endif // LINPUTBACKEND
