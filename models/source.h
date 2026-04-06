#ifndef SOURCE_H
#define SOURCE_H

#include "request.h"

class Source
{
private:
    int id;
    double alpha, beta;
    int numberForNewRequest;
    double nextTimeForGenerate;

    int countRequestsGenerated;
    int countRequestServed;
    int countRequestRejected;

public:
    Source(int id, double alpha, double beta);

    Request generateRequest(double currentTime);
    double scheduleNext(double currentTime);//?

    void increaseRequestsGenerated();
    void increaseRequestServed();
    void increaseRequestRejected();

    int getId() const;
    double getNextTimeForGenerate() const;
    int getCountRequestGenerated() const;
    int getCountRequestRejected() const;
    int getCountRequestServed() const;
};

#endif // SOURCE_H
