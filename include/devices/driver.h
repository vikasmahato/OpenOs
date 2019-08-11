#ifndef _KERNEL_DRIVER_H_
#define _KERNEL_DRIVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * The base class for all drivers.
 */
class Driver {
    public:
        Driver() {}
        ~Driver() {}
        void initialize();
        void reset();
        void destroy();
};

class DriverManager {
    private:
        /* The number of drivers currently loaded */
        int numDrivers;

        // TODO: This restricts us to only 256 drivers. Replace this with a linked list in the future
        Driver* drivers[256];
    public:
        DriverManager();
        /* Adds a new driver */
        void addDriver(Driver* driver);
        /* Initializes all drivers */
        void initializeAll();
};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_DRIVER_H_