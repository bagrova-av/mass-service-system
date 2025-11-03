#include "simulator.h"
#include <cmath>
#include <limits>
#include <algorithm>

//Constructors
Simulator::Simulator(size_t countSources, size_t countDevices, size_t bufferCapacity,
                    double alpha, double beta, double lambda, size_t targetRequestsCount) :
    time(0.0),
    buffer(bufferCapacity),
    dispatcher(),
    stats(countSources, countDevices),
    targetTotalRequestsProcessed(targetRequestsCount)
{
    generationFinished = false;
    for (size_t i = 0; i < countSources; ++i) {
        sources.emplace_back(i + 1, alpha, beta);
    }

    for (size_t i = 0; i < countDevices; ++i) {
        devices.emplace_back(i + 1, lambda);
    }
}

//Functions
void Simulator::init()
{
    time = 0.0;
    for (auto& src: sources)
    {
        src.scheduleNext(time);
    }
}

bool Simulator::isFinished() const
{
    // Генерация завершена И все заявки обслужены
    return generationFinished &&
           buffer.isEmpty() &&
           std::all_of(devices.begin(), devices.end(),
                       [](const Device& d) { return !d.getIsBusy(); });
}

void Simulator::nextEvent()
{
    // Если достигли целевого количества - останавливаем генерацию
    if (!generationFinished && stats.getTotalRequestsProcessed() >= targetTotalRequestsProcessed)
    {
        generationFinished = true;
    }

    // Если генерация завершена и система пуста - выходим
    if (isFinished())
    {
        return;
    }

    double minTime = std::numeric_limits<double>::infinity();
    int sourceIndex = -1;
    int deviceIndex = -1;
    double t = std::numeric_limits<double>::infinity();

    // Ищем следующее событие только среди активных источников (если генерация не завершена)
    if (!generationFinished)
    {
        for (size_t i = 0; i < sources.size(); ++i)
        {
            t = sources[i].getNextTimeForGenerate();
            if (t < minTime)
            {
                minTime = t;
                sourceIndex = static_cast<int>(i);
            }
        }
    }

    for (size_t i = 0; i < devices.size(); ++i)
    {
        t = devices[i].getTimeCompletion();
        if (t < minTime)
        {
            minTime = t;
            sourceIndex = -1;
            deviceIndex = static_cast<int>(i);
        }
        else if (std::fabs(t - minTime) < 1e-9)
        {
            // Одновременные события — приоритет освобождения прибора
            minTime = t;
            sourceIndex = -1;
            deviceIndex = static_cast<int>(i);
            break;
        }
    }

    // Если нет событий - выходим
    if (minTime == std::numeric_limits<double>::infinity())
    {
        return;
    }

    time = minTime;
    if (deviceIndex != -1)
    {
        // -------- Освобождение прибора --------
        auto& device = devices[deviceIndex];
        auto servedRequest = device.endServedRequest(time);

        if (servedRequest)
        {
            int srcId = servedRequest->getOnlySourceId();
            double timeInBuffer = servedRequest->getTimeStartService() - servedRequest->getTimeCreation();
            //Подумать про servedRequest->getTimeCreation() корректно ли это!!!!!
            double timeService = servedRequest->getTimeEndService() - servedRequest->getTimeStartService();
            stats.recordRequestsServiced(srcId, timeInBuffer, timeService);
            stats.recordDeviceBusy(device.getId(), timeService);
        }

        if (!buffer.isEmpty())
        {
            auto request = dispatcher.selectFromBuffer(buffer);
            if (request)
            {
                device.startServedRequest(request, time);
            }
        }

    }
    else if (sourceIndex != -1)
    {
        // -------- Генерация заявки источником --------
        auto& src = sources[sourceIndex];
        auto request = std::make_shared<Request>(src.generateRequest(time));
        stats.recordRequestsGenerated(src.getId());

        int deviceId = dispatcher.selectDevice(devices);
        if (deviceId != -1)
        {
            // Прибор свободен — сразу обслуживаем
            devices[deviceId].startServedRequest(request, time);
        }
        else
        {
            // Все приборы заняты — ставим заявку в буфер
            bool isPlacedInBuffer = dispatcher.placeToBuffer(buffer, request, time);
            if (!isPlacedInBuffer)
            {
                // Отказ (вытеснили самую старую)
                stats.recordRequestsRejected(src.getId());
            }
        }

        // Планируем следующую генерацию от этого источника
        src.scheduleNext(time);
    }
}

void Simulator::runAutoMode()
{
    while (!isFinished())
    {
        nextEvent();
    }
}

//Setters
void Simulator::setStatisticsTimeTotalModeling(double time)
{
    stats.setTimeTotalModeling(time);
}

//Getters
double Simulator::getTime() const
{
    return time;
}
int Simulator::getTargetTotalRequestsProcessed() const
{
    return targetTotalRequestsProcessed;
}
const std::vector<Source>& Simulator::getSources() const
{
    return sources;
}
const std::vector<Device>& Simulator::getDevices() const
{
    return devices;
}
const Buffer& Simulator::getBuffer() const
{
    return buffer;
}
const Statistics& Simulator::getStatistics() const
{
    return stats;
}
bool Simulator::isGenerationFinished() const
{
    return generationFinished;
}
