#ifndef DEVICE_H
#define DEVICE_H

#include <memory>
#include "request.h"

class Device
{
private:
    int id;
    double timeCompletion;
    bool isBusy;
    double timeTotalOccupied;
    double lambda; // intensity of exp distribution
    std::shared_ptr<Request> currentRequest;

public:
    Device(int id, double lambda);

    void startServedRequest(std::shared_ptr<Request> request, double currentTime);
    std::shared_ptr<Request> endServedRequest(double currentTime);

    int getId() const;
    double getTimeCompletion() const;
    bool getIsBusy() const;
    double getTimeTotalOccupied() const;
    std::shared_ptr<Request> getCurrentRequest() const;
};

#endif // DEVICE_H
