#include "dispatcher.h"

// Constuctors
Dispatcher::Dispatcher() :
    devicePtr(0) {}

// Functions
int Dispatcher::selectDevice(std::vector<Device>& devices)
{
    size_t countDevices = devices.size();
    for (size_t i = 0; i < countDevices; ++i)
    {
        size_t index = (devicePtr + i) % countDevices;
        if (!devices[index].getIsBusy())
        {
            devicePtr = (index + 1) % countDevices;
            return index;
        }
    }
    return -1;
}

bool Dispatcher::placeToBuffer(Buffer& buffer, std::shared_ptr<Request> request, double currentTime)
{
    if (!buffer.insertElement(request, currentTime))
    {
        buffer.replaceOldestElement(request, currentTime);
        return false; //reject the oldest request in the buffer
    }
    return true;
}

std::shared_ptr<Request> Dispatcher::selectFromBuffer(Buffer& buffer)
{
    return buffer.popElementLIFO();
}
