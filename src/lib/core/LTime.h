#ifndef LTIME_H
#define LTIME_H

#include <LNamespaces.h>

/**
 * @brief Time utilities
 */
class Louvre::LTime
{
public:
    /// @cond OMIT
    LTime() = delete;
    /// @endcond

    /**
     * @brief Milliseconds
     *
     * Time with a granularity of milliseconds and an undefined base.
     */
    static UInt32 ms();

    /**
     * @brief Nanoseconds
     *
     * Time with nanosecond granularity and undefined base.
     */
    static timespec ns();
};

#endif // LTIME_H
