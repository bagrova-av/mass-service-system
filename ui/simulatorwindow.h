#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "../app/simulator.h"

namespace Ui {
class SimulatorWindow;
}

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SimulatorWindow(int numSources, int numDevices, int bufferCapacity,
                             double alpha, double beta, double lambda, int target,
                             QWidget *parent = nullptr);

    ~SimulatorWindow();

private slots:
    void on_nextStepButton_clicked();
    void on_backButton_clicked();
    void updateView();
    void on_resultsShowButton_clicked();

    void on_mode_currentChanged(int index);

    void on_showTablesButton_clicked();
    void on_autoModeTimer();
    void on_startAutoButton_clicked();
    void on_stopAutoButton_clicked();

private:
    struct ExperimentResult {
        int xValue;               // число приборов / источников / буфера
        double pRefuse;
        double tSystem;
        double usage;
    };

    Ui::SimulatorWindow *ui;
    Simulator simulator;
    QTimer timerForStepMode;
    QTimer timerForAutoMode;
    bool autoMode = false;

    void startAutomaticMode();
    void stopAutomaticMode();
    void buildCharts(const Statistics& stats);
    void setupTables();
    void updateSourcesTable();
    void updateDevicesTable();
    void updateBufferTable();

    void updateAutoView();

    int estimateRequiredRequests(int numSources, int numDevices, int bufferSize,
        double alpha, double beta, double lambda);

    void runExperimentsByDevices();
    void runExperimentsByBuffer();
    void runExperimentsBySources();
    void buildChartsByDevices(const std::vector<ExperimentResult>& results);
    void buildChartsByBuffer(const std::vector<ExperimentResult>& results);
    void buildChartsBySources(const std::vector<ExperimentResult>& results);
};

#endif // SIMULATORWINDOW_H
