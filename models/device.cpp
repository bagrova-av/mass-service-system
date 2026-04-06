#include "device.h"
#include <cmath>
#include <random>

//Constuctors
Device::Device(int id, double lambda) :
    id(id),
    isBusy(false),
    timeCompletion(INFINITY),
    timeTotalOccupied(0.0),
    lambda(lambda),
    currentRequest(nullptr) {}

//Functions
void Device::startServedRequest(std::shared_ptr<Request> request, double currentTime)
{
    isBusy = true;
    currentRequest = request;

    static std::mt19937 gen(std::random_device{}());
    // std::uniform_real_distribution<> dist(0.0, 1.0);
    // double U = dist(gen);
    // double serviceTime = -std::log(U) / lambda;
    std::exponential_distribution<> dist(lambda);
    double timeService = dist(gen);

    request->setTimeStart(currentTime);
    timeCompletion = currentTime + timeService;
    timeTotalOccupied += timeService;
}
std::shared_ptr<Request> Device::endServedRequest(double currentTime)
{
    isBusy = false;
    timeCompletion = INFINITY;
    auto servedRequest = currentRequest;
    if (servedRequest)
    {
        servedRequest->setTimeEnd(currentTime);
    }
    currentRequest.reset();
    return servedRequest;
}

//Geters
int Device::getId() const
{
    return id;
}
double Device::getTimeCompletion() const
{
    return timeCompletion;
}
bool Device::getIsBusy() const
{
    return isBusy;
}
double Device::getTimeTotalOccupied() const
{
    return timeTotalOccupied;
}
 std::shared_ptr<Request> Device::getCurrentRequest() const
{
     return currentRequest;
}
