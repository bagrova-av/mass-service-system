#include "source.h"

#include <random>
#include <iostream>

//Costructors
Source::Source(int id, double alpha, double beta) :
    id(id),
    alpha(alpha),
    beta(beta),
    numberForNewRequest(0),
    nextTimeForGenerate(0),
    countRequestsGenerated(0),
    countRequestServed(0),
    countRequestRejected(0) {}

//Fucntions
Request Source::generateRequest(double currentTime)
{
    countRequestsGenerated++;
    numberForNewRequest++;
    std::cout << "Source " << id << " generating request #" << numberForNewRequest
              << " at time " << currentTime << '\n';
    return Request(id, numberForNewRequest, currentTime);
}
double Source::scheduleNext(double currentTime)
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(alpha, beta);
    nextTimeForGenerate = currentTime + dist(gen);
    return nextTimeForGenerate;
}

void Source::increaseRequestsGenerated()
{
    countRequestsGenerated++;
}
void Source::increaseRequestServed()
{
    countRequestServed++;
}
void Source::increaseRequestRejected()
{
    countRequestRejected++;
}

//Getters
int Source::getId() const
{
    return id;
}
double Source::getNextTimeForGenerate() const
{
    return nextTimeForGenerate;
}
int Source::getCountRequestGenerated() const
{
    return countRequestsGenerated;
}
int Source::getCountRequestServed() const
{
    return countRequestServed;
}
int Source::getCountRequestRejected() const
{
    return countRequestRejected;
}

