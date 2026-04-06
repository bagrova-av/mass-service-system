#include "request.h"

// Constuctors
Request::Request(int sourceId, int requestId, double timeCreation) :
    id(std::make_pair(sourceId, requestId)),
    timeCreation(timeCreation),
    timeStartService(-1),
    timeEndService(-1) {}

// Setters
void Request::setTimeStart(double timeStart)
{
    timeStartService = timeStart;
}
void Request::setTimeEnd(double timeEnd)
{
    timeEndService = timeEnd;
}

// Getters
std::pair<int, int> Request::getId() const
{
    return id;
}
int Request::getOnlySourceId() const
{
    return id.first;
}
int Request::getOnlyRequestId() const
{
    return id.second;
}
double Request::getTimeCreation() const
{
    return timeCreation;
}
double Request::getTimeStartService() const
{
    return timeStartService;
}
double Request::getTimeEndService() const
{
    return timeEndService;
}
