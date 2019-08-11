#include <devices/driver.h>

void Driver::initialize() {}
void Driver::reset() {}
void Driver::destroy() {}

DriverManager::DriverManager()
{
    numDrivers = 0;
}

void DriverManager::addDriver(Driver* driver)
{
    drivers[numDrivers] = driver;
    numDrivers++;
}

void DriverManager::initializeAll()
{
    for(int i = 0; i < numDrivers; i++)
        drivers[i]->initialize();
}