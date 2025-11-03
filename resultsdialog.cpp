#include "resultsdialog.h"
#include "ui_resultsdialog.h"

#include <iostream>

void ResultsDialog::setupSourcesTable()
{
    std::cout << "=== Starting setupSourcesTable ===" << std::endl;

    int sourceCount = statisticsRef.getSourcesCount();
    std::cout << "Source count: " << sourceCount << std::endl;

    ui->tableSourcesResults->setRowCount(sourceCount);

    for (int i = 0; i < sourceCount; ++i) {
        int sourceId = i + 1;
        std::cout << "Processing source " << sourceId << " (i=" << i << ")..." << std::endl;

        try {
            // 1. Создаем ячейку для ID
            std::cout << "  Creating ID cell..." << std::endl;
            ui->tableSourcesResults->setItem(i, 0, new QTableWidgetItem(QString::number(sourceId)));

            // 2. Получаем статистику
            std::cout << "  Getting source stats..." << std::endl;
            const auto& sourceStats = statisticsRef.getSourceStats(sourceId);
            std::cout << "  Got stats, generated: " << sourceStats.countRequestsGenerated << std::endl;

            // 3. Создаем ячейку для сгенерированных заявок
            std::cout << "  Creating generated requests cell..." << std::endl;
            ui->tableSourcesResults->setItem(i, 1,
                                             new QTableWidgetItem(QString::number(sourceStats.countRequestsGenerated)));

            // 4. Вычисляем вероятность отказа
            std::cout << "  Calculating rejection probability..." << std::endl;
            double rejectProb = statisticsRef.calcRejectedProbability(sourceId);
            ui->tableSourcesResults->setItem(i, 2,
                                             new QTableWidgetItem(QString::number(rejectProb, 'f', 4)));

            // 5. Остальные вычисления - делаем по одному с проверкой
            std::cout << "  Calculating avg buffer time..." << std::endl;
            double avgBufferTime = statisticsRef.calcAvgInBuffer(sourceId);
            ui->tableSourcesResults->setItem(i, 3,
                                             new QTableWidgetItem(QString::number(avgBufferTime, 'f', 4)));

            std::cout << "  Calculating avg service time..." << std::endl;
            double avgServiceTime = statisticsRef.calcAvgService(sourceId);
            ui->tableSourcesResults->setItem(i, 4,
                                             new QTableWidgetItem(QString::number(avgServiceTime, 'f', 4)));

            std::cout << "  Calculating avg system time..." << std::endl;
            double avgSystemTime = statisticsRef.calcAvgInSystem(sourceId);
            ui->tableSourcesResults->setItem(i, 5,
                                             new QTableWidgetItem(QString::number(avgSystemTime, 'f', 4)));

            std::cout << "  Calculating buffer dispersion..." << std::endl;
            double dispBuffer = statisticsRef.calcDispInBuffer(sourceId);
            ui->tableSourcesResults->setItem(i, 6,
                                             new QTableWidgetItem(QString::number(dispBuffer, 'f', 4)));

            std::cout << "  Calculating service dispersion..." << std::endl;
            double dispService = statisticsRef.calcDispService(sourceId);
            ui->tableSourcesResults->setItem(i, 7,
                                             new QTableWidgetItem(QString::number(dispService, 'f', 4)));

            std::cout << "  Source " << sourceId << " completed successfully!" << std::endl;

        } catch (const std::exception& e) {
            std::cout << "  ERROR processing source " << sourceId << ": " << e.what() << std::endl;
            throw; // перебрасываем исключение дальше
        }
    }

    std::cout << "=== setupSourcesTable completed successfully ===" << std::endl;
}

void ResultsDialog::setupDevicesTable()
{
    std::cout << "=== Starting setupDevicesTable ===" << std::endl;

    int deviceCount = statisticsRef.getDevicesCount();
    std::cout << "Device count: " << deviceCount << std::endl;

    ui->tableDevicesResults->setRowCount(deviceCount);

    for (int i = 0; i < deviceCount; ++i) {
        int deviceId = i + 1;
        std::cout << "Processing device " << deviceId << " (i=" << i << ")..." << std::endl;

        try {
            ui->tableDevicesResults->setItem(i, 0,
                                             new QTableWidgetItem(QString::number(deviceId)));

            std::cout << "  Getting device stats..." << std::endl;

            double busyTime = statisticsRef.getDeviceStats(deviceId).timeTotalBusy;
            std::cout << "  busyTime = " << busyTime << std::endl;
            ui->tableDevicesResults->setItem(i, 1,
                                             new QTableWidgetItem(QString::number(busyTime, 'f', 4)));

            std::cout << "  Calculating device usage..." << std::endl;
            double usage = statisticsRef.calcDeviceUsage(deviceId);
            std::cout << "  usage = " << usage << std::endl;
            ui->tableDevicesResults->setItem(i, 2,
                                             new QTableWidgetItem(QString::number(usage, 'f', 4)));


            std::cout << "  Device " << deviceId << " completed successfully!" << std::endl;

        } catch (const std::exception& e) {
            std::cout << "  ERROR processing device " << deviceId << ": " << e.what() << std::endl;
            throw;
        }
    }

    std::cout << "=== setupDevicesTable completed successfully ===" << std::endl;
}


ResultsDialog::ResultsDialog(const Statistics& stats, double totalTime, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog),
    statisticsRef(stats),
    totalModelingTime(totalTime)
{
    ui->setupUi(this);
    setupSourcesTable();
    setupDevicesTable();
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}


void ResultsDialog::on_closeResultsButton_clicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

