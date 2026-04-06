#include "statistics.h"
#include <iostream>

//Constructors
Statistics::Statistics(int countSources, int countDevices) :
    sourcesStatics(countSources),
    devicesStatics(countDevices) {}

//Functions
void Statistics::recordRequestsGenerated(int sourceId)
{
    sourcesStatics[sourceId-1].countRequestsGenerated++;
}
void Statistics::recordRequestsRejected(int sourceId)
{
    sourcesStatics[sourceId-1].countRequestsRejected++;
}
void Statistics::recordRequestsServiced(int sourceId, double timeInBuffer, double timeService)
{
    auto& source = sourcesStatics[sourceId - 1];
    source.countRequestsServed++;
    source.timeTotalInBuffer += timeInBuffer;
    source.timeTotalService += timeService;
    source.timeTotalInSystem += timeInBuffer + timeService;
    source.timesInBuffer.push_back(timeInBuffer);
    source.timesService.push_back(timeService);
    totalProcessedRequests++;
}
void Statistics::recordDeviceBusy(int deviceId, double timeBusy)
{
    devicesStatics[deviceId-1].timeTotalBusy += timeBusy;
}

double Statistics::calcRejectedProbability(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.countRequestsGenerated == 0)
    {
        return 0.0;
    }
    return static_cast<double>(src.countRequestsRejected) / src.countRequestsGenerated;
}
double Statistics::calcAvgInBuffer(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.countRequestsServed == 0)
    {
        return 0.0;
    }
    return src.timeTotalInBuffer / src.countRequestsServed;
}
double Statistics::calcAvgService(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.countRequestsServed == 0)
    {
        return 0.0;
    }
    return src.timeTotalService / src.countRequestsServed;
}
double Statistics::calcAvgInSystem(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.countRequestsServed == 0)
    {
        return 0.0;
    }
    return src.timeTotalInSystem / src.countRequestsServed;
}

double Statistics::calcDispInBuffer(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.timesInBuffer.size() <= 1) return 0.0;

    double mean = calcAvgInBuffer(sourceId);
    double sum = 0.0;
    for (double time : src.timesInBuffer)
    {
        sum += (time - mean) * (time - mean);
    }
    return sum / src.timesInBuffer.size();
}
double Statistics::calcDispService(int sourceId) const
{
    const auto& src = sourcesStatics[sourceId-1];
    if (src.timesService.size() <= 1) return 0.0;

    double mean = calcAvgService(sourceId);
    double sum = 0.0;
    for (double time : src.timesService)
    {
        sum += (time - mean) * (time - mean);
    }
    return sum / src.timesService.size();
}

double Statistics::calcDeviceUsage(int deviceId) const
{
    if (timeTotalModeling <= 1e-9)
    {
        return 0.0;
    }
    return devicesStatics[deviceId-1].timeTotalBusy / timeTotalModeling;
}


// Setters
void Statistics::setTimeTotalModeling(double time)
{
    timeTotalModeling = time;
}

//Getters
int Statistics::getTotalRequestsProcessed() const
{
    return totalProcessedRequests;
}
const SourceStatistics& Statistics::getSourceStats(int sourceId) const
{
    return sourcesStatics[sourceId - 1];
}
const DeviceStatistics& Statistics::getDeviceStats(int deviceId) const
{
    return devicesStatics[deviceId - 1];
}
int Statistics::getSourcesCount() const
{
    return sourcesStatics.size();
}
int Statistics::getDevicesCount() const
{
    return devicesStatics.size();
}

void Statistics::printSourceStats() const
{
    std::cout << "\n=== Характеристики источников ===\n";
    std::cout << "Источник | Сгенер. | Обраб. | Отказ. | Pотк | \
| Tпреб | Tбп | Tобсл | Dобсл | Dбп\n";

    for (size_t i = 0; i < sourcesStatics.size(); ++i)
    {
        std::cout << "   " << i + 1 << "      "
                  << sourcesStatics[i].countRequestsGenerated << "       "
                  << sourcesStatics[i].countRequestsServed << "      "
                  << sourcesStatics[i].countRequestsRejected << "    "
                  << calcRejectedProbability(i) << "   "
                  << calcAvgInSystem(i) << "   "
                  << calcAvgInBuffer(i) << "   "
                  << calcAvgService(i) << "   "
                  << calcDispService(i) << "   "
                  << calcDispInBuffer(i) << "\n";
    }
}

void Statistics::printDeviceStats() const
{
    std::cout << "\n=== Характеристики приборов ===\n";
    std::cout << "Прибор | Коэф. использования\n";
    for (size_t i = 0; i < devicesStatics.size(); ++i)
    {
        std::cout << "   " << i + 1 << "      "
                  << calcDeviceUsage(i) << "\n";
    }
}
