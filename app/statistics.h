#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>

struct SourceStatistics
{
    int countRequestsGenerated = 0;
    int countRequestsRejected = 0;
    int countRequestsServed = 0;
    double timeTotalInBuffer = 0.0;
    double timeTotalService = 0.0;
    double timeTotalInSystem = 0.0;
    std::vector<double> timesInBuffer;  // для дисперсии ожидания
    std::vector<double> timesService;  // для дисперсии обслуживания
};

struct DeviceStatistics
{
    double timeTotalBusy = 0.0;
};

class Statistics
{
private:
    std::vector<SourceStatistics> sourcesStatics;
    std::vector<DeviceStatistics> devicesStatics;

    int totalProcessedRequests = 0;
    double timeTotalModeling = 0.0;

public:
    Statistics(int countSources, int countDevices);

    void recordRequestsGenerated(int sourceId);
    void recordRequestsRejected(int sourceId);
    void recordRequestsServiced(int sourceId, double timeInBuffer, double timeService);
    void recordDeviceBusy(int deviceId, double timeBusy);

    void setTimeTotalModeling(double time);

    double calcRejectedProbability(int sourceId) const;
    double calcAvgInBuffer(int sourceId) const;
    double calcAvgService(int sourceId) const;
    double calcAvgInSystem(int sourceId) const;
    double calcDispInBuffer(int sourceId) const;
    double calcDispService(int sourceId) const;
    double calcDeviceUsage(int deviceId) const;

    int getTotalRequestsProcessed() const;
    const SourceStatistics& getSourceStats(int sourceId) const;
    const DeviceStatistics& getDeviceStats(int deviceId) const;
    int getSourcesCount() const;
    int getDevicesCount() const;

    void printSourceStats() const;
    void printDeviceStats() const;
};

#endif // STATISTICS_H
