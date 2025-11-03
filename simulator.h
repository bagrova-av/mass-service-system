#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "source.h"
#include "dispatcher.h"
#include "statistics.h"

class Simulator
{
private:
    double time;
    std::vector<Source> sources;
    std::vector<Device> devices;
    Buffer buffer;
    Dispatcher dispatcher;
    Statistics stats;

    int targetTotalRequestsProcessed;
    bool generationFinished;

public:
    Simulator(size_t countSources, size_t countDevices, size_t bufferCapacity,
            double alpha, double beta, double lambda, size_t targetRequestsCount);

    void init();
    void nextEvent();
    void runAutoMode();
    void setStatisticsTimeTotalModeling(double time);

    bool isFinished() const;
    bool isGenerationFinished() const;

    double getTime() const;
    int getTargetTotalRequestsProcessed() const;
    const std::vector<Source>& getSources() const;
    const std::vector<Device>& getDevices() const;
    const Buffer& getBuffer() const;
    const Statistics& getStatistics() const;
};

#endif // SIMULATOR_H
