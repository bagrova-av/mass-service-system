#ifndef REQUEST_H
#define REQUEST_H

#include <utility>
class Request
{
private:
    std::pair<int, int> id; // (sourceId, requestId)
    double timeCreation;
    double timeStartService;
    double timeEndService;

public:
    Request(int sourceId, int requestId, double timeCreation);

    std::pair<int, int> getId() const;
    int getOnlySourceId() const;
    int getOnlyRequestId() const;
    double getTimeCreation() const;
    double getTimeStartService() const;
    double getTimeEndService() const;

    void setTimeStart(double timeStart);
    void setTimeEnd(double timeEnd);
};

#endif // REQUEST_H
