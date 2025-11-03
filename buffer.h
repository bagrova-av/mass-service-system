#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <memory>
#include "request.h"

struct BufferSlot
{
    bool isOccupied;
    std::shared_ptr<Request> request;
    double timeEntry;

    BufferSlot() :
        isOccupied(false),
        request(nullptr),
        timeEntry(-1) {}
};

class Buffer
{
private:
    int capacity;
    std::vector<BufferSlot> bufferSlots;
    int pointer; //next place after last occupied
    int countOccupiedSlots; // current number of requests

public:
    Buffer(int capacity);

    bool insertElement(std::shared_ptr<Request> request, double currentTime);
    bool replaceOldestElement(std::shared_ptr<Request> request, double currentTime);
    std::shared_ptr<Request> popElementLIFO();

    bool isEmpty() const;
    int getCountOccupiedSlots() const;
    int getCapacity() const;

    const std::vector<BufferSlot>& getSlots() const; // for GUI
};

#endif // BUFFER_H
