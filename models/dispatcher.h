#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "device.h"
#include "buffer.h"

class Dispatcher
{
private:
    int devicePtr; // round-robin pointer

public:
    Dispatcher();

    int selectDevice(std::vector<Device>& devices);
    bool placeToBuffer(Buffer& buffer, std::shared_ptr<Request> request, double currentTime);
    std::shared_ptr<Request> selectFromBuffer(Buffer& buffer);
};

#endif // DISPATCHER_H
