#include "buffer.h"
#include <cmath>

//Constuctors
Buffer::Buffer(int capacity) :
    capacity(capacity),
    pointer(0),
    countOccupiedSlots(0)
{
    bufferSlots.resize(capacity);
}

//Functions
bool Buffer::insertElement(std::shared_ptr<Request> request, double currentTime)
{
    for (int i = 0; i < capacity; ++i)
    {
        int index = (pointer + i) % capacity;
        if (!bufferSlots[index].isOccupied)
        {
            bufferSlots[index].isOccupied = true;
            bufferSlots[index].request = request;
            bufferSlots[index].timeEntry = currentTime;

            countOccupiedSlots++;
            pointer = (index + 1) % capacity;
            return true;
        }
    }
    return false;
}
bool Buffer::replaceOldestElement(std::shared_ptr<Request> request, double currentTime)
{
    if (countOccupiedSlots == 0)
    {
        return false;
    }

    int oldestElementIndex = -1;
    double oldestElementTime = INFINITY;
    for (int i = 0; i < capacity; ++i)
    {
        if (bufferSlots[i].isOccupied && bufferSlots[i].timeEntry < oldestElementTime)
        {
            oldestElementTime = bufferSlots[i].timeEntry;
            oldestElementIndex = i;
        }
    }
    if (oldestElementIndex == -1)
    {
        return false;
    }

    bufferSlots[oldestElementIndex].request = request;
    bufferSlots[oldestElementIndex].timeEntry = currentTime;
    pointer = (oldestElementIndex + 1) % capacity;
    return true;
}
std::shared_ptr<Request> Buffer::popElementLIFO()
{
    if (countOccupiedSlots == 0)
    {
        return nullptr;
    }

    int newestElementIndex = -1;
    double newestElementTime = -1;
    for (int i = 0; i < capacity; ++i)
    {
        if (bufferSlots[i].isOccupied && bufferSlots[i].timeEntry > newestElementTime) {
            newestElementIndex = i;
            newestElementTime = bufferSlots[i].timeEntry;
        }
    }
    if (newestElementIndex == -1)
    {
        return nullptr;
    }

    auto request = bufferSlots[newestElementIndex].request;
    bufferSlots[newestElementIndex].isOccupied = false;
    bufferSlots[newestElementIndex].request.reset();
    bufferSlots[newestElementIndex].timeEntry = -1;
    countOccupiedSlots--;

    return request;
}

//Getters
bool Buffer::isEmpty() const
{
    return countOccupiedSlots == 0;
}
int Buffer::getCountOccupiedSlots() const
{
    return countOccupiedSlots;
}
int Buffer::getCapacity() const
{
    return capacity;
}
const std::vector<BufferSlot>& Buffer::getSlots() const
{
    return bufferSlots;
}
