#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>
#include <QtCharts>

#include "resultsdialog.h"

void SimulatorWindow::setupTables()
{
    // Настройка таблицы источников
    ui->tableSources->setRowCount(simulator.getSources().size());
    ui->tableSources->horizontalHeader()->setStretchLastSection(true);

    // Настройка таблицы приборов
    ui->tableDevices->setRowCount(simulator.getDevices().size());

    // Настройка таблицы буфера
    ui->tableBuffer->setRowCount(simulator.getBuffer().getCapacity());
    for (int i = 0; i < simulator.getBuffer().getCapacity(); ++i)
    {
        ui->tableBuffer->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    }
}

SimulatorWindow::SimulatorWindow(int numSources, int numDevices, int bufferCapacity,
                                 double alpha, double beta, double lambda, int target,
                                 QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::SimulatorWindow),
    simulator(numSources, numDevices, bufferCapacity, alpha, beta, lambda, target)
{
    ui->setupUi(this);
    simulator.init();

    if (numSources != 0 && numDevices !=0 && bufferCapacity != 0)
    {
        int N_opt = estimateRequiredRequests(numSources, numDevices, bufferCapacity, alpha, beta, lambda);
        qDebug() << "Оптимальное количество заявок:" << N_opt;
    }

    // Настройка таблиц
    setupTables();
    updateSourcesTable();

    connect(&timerForStepMode, &QTimer::timeout, this, [this]()
    {
        if (!simulator.isFinished())
        {
            simulator.nextEvent();
            updateView();
        }
        else
        {
            timerForStepMode.stop();
            simulator.setStatisticsTimeTotalModeling(simulator.getTime());
            QMessageBox::information(this, "Симуляция завершена",
                                     "Обработано все целевые заявки");
        }
    });

    connect(&timerForAutoMode, &QTimer::timeout, this, &SimulatorWindow::on_autoModeTimer);
    connect(ui->startAutoButton, &QPushButton::clicked, this, &SimulatorWindow::on_startAutoButton_clicked);
    connect(ui->stopAutoButton, &QPushButton::clicked, this, &SimulatorWindow::on_stopAutoButton_clicked);

    updateView();
    updateAutoView();
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
}

void SimulatorWindow::updateSourcesTable()
{
    const auto& sources = simulator.getSources();
    for (size_t i = 0; i < sources.size(); ++i)
    {
        const auto& source = sources[i];

        // ID источника
        if (ui->tableSources->item(i, 0) == nullptr)
        {
            ui->tableSources->setItem(i, 0, new QTableWidgetItem());
        }
        ui->tableSources->item(i, 0)->setText(QString::number(source.getId()));

        // Время следующей генерации
        if (ui->tableSources->item(i, 1) == nullptr)
        {
            ui->tableSources->setItem(i, 1, new QTableWidgetItem());
        }
        ui->tableSources->item(i, 1)->setText(
            QString::number(source.getNextTimeForGenerate(), 'f', 4));
    }
}

void SimulatorWindow::updateDevicesTable()
{
    const auto& devices = simulator.getDevices();
    for (size_t i = 0; i < devices.size(); ++i)
    {
        const auto& device = devices[i];

        // Состояние прибора
        if (ui->tableDevices->item(i, 0) == nullptr)
        {
            ui->tableDevices->setItem(i, 0, new QTableWidgetItem());
        }
        ui->tableDevices->item(i, 0)->setText(
            device.getIsBusy() ? "Занят" : "Свободен");

        // Текущая заявка
        if (ui->tableDevices->item(i, 1) == nullptr)
        {
            ui->tableDevices->setItem(i, 1, new QTableWidgetItem());
        }
        if (device.getIsBusy() && device.getCurrentRequest()) {
            auto request = device.getCurrentRequest();
            ui->tableDevices->item(i, 1)->setText("S" + QString::number(request->getOnlySourceId()) + " - "
                                                  + QString::number(request->getOnlyRequestId()));
        }
        else
        {
            ui->tableDevices->item(i, 1)->setText("-");
        }

        // Время окончания обслуживания
        if (ui->tableDevices->item(i, 2) == nullptr) {
            ui->tableDevices->setItem(i, 2, new QTableWidgetItem());
        }
        if (device.getIsBusy()) {
            ui->tableDevices->item(i, 2)->setText(
                QString::number(device.getTimeCompletion(), 'f', 4));
        }
        else
        {
            ui->tableDevices->item(i, 2)->setText("-");
        }
    }
}

void SimulatorWindow::updateBufferTable()
{
    const auto& buffer = simulator.getBuffer();
    const auto& bufferSlots = buffer.getSlots();

    for (size_t i = 0; i < bufferSlots.size(); ++i)
    {
        const auto& bufferSlot = bufferSlots[i];

        // Номер места (уже установлен в setupTables)

        // Номер заявки
        if (ui->tableBuffer->item(i, 1) == nullptr)
        {
            ui->tableBuffer->setItem(i, 1, new QTableWidgetItem());
        }
        if (bufferSlot.isOccupied && bufferSlot.request)
        {
            ui->tableBuffer->item(i, 1)->setText(
                QString::number(bufferSlot.request->getOnlyRequestId()));
        }
        else
        {
            ui->tableBuffer->item(i, 1)->setText("-");
        }

        // Номер источника
        if (ui->tableBuffer->item(i, 2) == nullptr)
        {
            ui->tableBuffer->setItem(i, 2, new QTableWidgetItem());
        }
        if (bufferSlot.isOccupied && bufferSlot.request)
        {
            ui->tableBuffer->item(i, 2)->setText(
                QString::number(bufferSlot.request->getOnlySourceId()));
        }
        else
        {
            ui->tableBuffer->item(i, 2)->setText("-");
        }
    }
}

void SimulatorWindow::updateView()
{
    // Обновление времени и счетчика заявок
    ui->labelCurrentTime->setText( QString::number(simulator.getTime(), 'f', 4) );
    ui->labelProcessedRequests->setText( QString::number(simulator.getStatistics().getTotalRequestsProcessed()));

    // Обновление прогресс-бара
    int progress = static_cast<int>((static_cast<double>(
                                    simulator.getStatistics().getTotalRequestsProcessed()) /
                                     simulator.getTargetTotalRequestsProcessed()) * 100);
    ui->progressBar->setValue(progress);

    // Обновление таблицы источников
    updateSourcesTable();

    // Обновление таблицы приборов
    updateDevicesTable();

    // Обновление таблицы буфера
    updateBufferTable();

    // Отображаем состояние генерации
    if (simulator.isGenerationFinished())
    {
        ui->labelGenerationStatus->setText("Генерация заявок завершена, обслуживание оставшихся заявок...");
    }
    else
    {
        ui->labelGenerationStatus->setText("Генерация активна");
    }
}



void SimulatorWindow::on_nextStepButton_clicked()
{
    if (!simulator.isFinished())
    {
        simulator.nextEvent();
        updateView();
    }
    else
    {
        QMessageBox::information(this, "Симуляция завершена",
                                 QString("Все заявки обслужены!\n"
                                         "Время моделирования: %1\n"
                                         "Обработано заявок: %2")
                                     .arg(simulator.getTime(), 0, 'f', 4)
                                     .arg(simulator.getStatistics().getTotalRequestsProcessed()));
    }
}

// void SimulatorWindow::on_autoRunButton_clicked()
// {
//     if (!autoMode)
//     {
//         autoMode = true;
//         timer.start(500);  // шаг каждые 500 мс
//         ui->autoRunButton->setText("Стоп");
//     }
//     else {
//         autoMode = false;
//         timer.stop();
//         ui->autoRunButton->setText("Автоматически");
//     }
// }

void SimulatorWindow::on_backButton_clicked()
{
    this->close();
    if (parentWidget())
    {
        parentWidget()->show();
    }
}

void SimulatorWindow::on_resultsShowButton_clicked()
{
    qDebug() << "=== DEBUG: Starting results dialog ===";

    simulator.setStatisticsTimeTotalModeling(simulator.getTime());

    // Проверяем, что статистика доступна
    qDebug() << "Sources count:" << simulator.getStatistics().getSourcesCount();
    qDebug() << "Devices count:" << simulator.getStatistics().getDevicesCount();
    qDebug() << "Total time:" << simulator.getTime();
    qDebug() << "Total processed:" << simulator.getStatistics().getTotalRequestsProcessed();

    try {
        // Создаем и показываем диалог с результатами
        ResultsDialog resultsDialog(simulator.getStatistics(),
                                    simulator.getTime(),
                                    this);
        qDebug() << "ResultsDialog created successfully";
        resultsDialog.exec();
        qDebug() << "ResultsDialog closed";
    }
    catch (const std::exception& e) {
        qDebug() << "Exception:" << e.what();
        QMessageBox::critical(this, "Ошибка", QString("Ошибка при создании диалога: %1").arg(e.what()));
    }
    catch (...) {
        qDebug() << "Unknown exception";
        QMessageBox::critical(this, "Ошибка", "Неизвестная ошибка при создании диалога");
    }

    qDebug() << "=== DEBUG: Results dialog finished ===";
}


void SimulatorWindow::on_mode_currentChanged(int index)
{
    if (ui->tabMode->widget(index) == ui->autoModeTab)
    {
        if (!autoMode && !simulator.isFinished())
        {
            startAutomaticMode();
        }
    }
    else
    {
        if (autoMode)
        {
            stopAutomaticMode();
        }
    }
}

void SimulatorWindow::startAutomaticMode()
{
    simulator.runAutoMode();
    timerForAutoMode.start(30);

    const auto& stats = simulator.getStatistics();

    //buildCharts(stats);
}

void SimulatorWindow::stopAutomaticMode()
{
    autoMode = false;
    timerForAutoMode.stop();
}

void SimulatorWindow::on_showTablesButton_clicked()
{
    simulator.setStatisticsTimeTotalModeling(simulator.getTime());
    ResultsDialog resultsDialog(simulator.getStatistics(),
                                simulator.getTime(),
                                this);
    qDebug() << "ResultsDialog created successfully";
    resultsDialog.exec();
}


void SimulatorWindow::on_autoModeTimer()
{
    if (!simulator.isFinished()) {
        simulator.nextEvent();
        updateAutoView();
    } else {
        on_stopAutoButton_clicked();
    }
}

void SimulatorWindow::updateAutoView()
{
    // Обновление для автоматического режима
    ui->labelAutoTime->setText(QString::number(simulator.getTime(), 'f', 4));
    ui->labelAutoProcessed->setText(QString::number(simulator.getStatistics().getTotalRequestsProcessed()));

    if (autoMode) {
        if (simulator.isFinished()) {
            ui->labelAutoStatus->setText("Статус: Завершено");
            ui->startAutoButton->setEnabled(false);
            ui->stopAutoButton->setEnabled(false);
            ui->showTablesButton->setEnabled(true);
            timerForAutoMode.stop();
        } else {
            ui->labelAutoStatus->setText("Статус: Выполняется...");
        }
    }
}

void SimulatorWindow::on_startAutoButton_clicked()
{
    autoMode = true;
    ui->startAutoButton->setEnabled(false);
    ui->stopAutoButton->setEnabled(true);
    ui->showTablesButton->setEnabled(false);
    ui->labelAutoStatus->setText("Статус: Выполняется...");

    // Запускаем таймер с быстрым обновлением (50 мс)
    timerForAutoMode.start(50);

    runExperimentsByDevices();
    runExperimentsByBuffer();
    runExperimentsBySources();
}


void SimulatorWindow::on_stopAutoButton_clicked()
{
    autoMode = false;
    ui->startAutoButton->setEnabled(true);
    ui->stopAutoButton->setEnabled(false);
    ui->showTablesButton->setEnabled(true);
    ui->labelAutoStatus->setText("Статус: Остановлено пользователем");
    timerForAutoMode.stop();
}

static QChart* createLineChart(
    const QString &title,
    const QString &xTitle,
    const QString &yTitle,
    const std::vector<QPointF> &points)
{
    QLineSeries *series = new QLineSeries();
    for (const auto &p : points)
        series->append(p.x(), p.y());

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);

    // Настраиваем ось X (целочисленная)
    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelFormat("%d");
    axisX->setTitleText(xTitle);

    // int maxX = points.empty() ? 1 : static_cast<int>(points.back().x());
    // axisX->setRange(1, maxX);
    int minX = points.empty() ? 1 : static_cast<int>(points.front().x());
    int maxX = points.empty() ? 1 : static_cast<int>(points.back().x());
    axisX->setRange(minX, maxX);
    axisX->setTickType(QValueAxis::TicksFixed);
    axisX->setTickAnchor(minX);
    axisX->setTickInterval(1.0);
    axisX->setTickCount(maxX - minX + 1);
    axisX->setTickType(QValueAxis::TicksFixed);
    axisX->setMinorTickCount(0);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Настраиваем ось Y (двухстрочная)
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.3f");
    axisY->setTitleText(yTitle);

    if (!points.empty()) {
        double minY = points[0].y(), maxY = points[0].y();
        for (const auto &p : points) {
            if (p.y() < minY) minY = p.y();
            if (p.y() > maxY) maxY = p.y();
        }
        if (minY == maxY) { // все точки равны
            minY -= 0.1; maxY += 0.1;
        }
        axisY->setRange(minY, maxY);
    } else {
        axisY->setRange(0, 1);
    }
    axisY->setTickType(QValueAxis::TicksFixed);
    axisY->setTickCount(9);
    //axisY->setMinorTickCount(1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);
    return chart;
}
inline QPointF makePoint(int x, double y) {
    return QPointF(static_cast<qreal>(x), static_cast<qreal>(y));
}


void SimulatorWindow::runExperimentsByDevices()
{
    std::vector<ExperimentResult> results;

    const int numSources = 11;
    const int bufferSize = 4;
    const double alpha = 5, beta = 7, lambda = 0.6;
    const int target = 4000;

    for (int nDevices = 1; nDevices <= 6; ++nDevices)
    {
        Simulator sim(numSources, nDevices, bufferSize, alpha, beta, lambda, target);
        sim.init();
        sim.runAutoMode();
        sim.setStatisticsTimeTotalModeling(sim.getTime());
        const Statistics& statics = sim.getStatistics();

        // --- собираем средние характеристики ---
        double avgPRefuse = 0, avgTSystem = 0, avgUsage = 0;

        for (int i = 1; i <= numSources; ++i) {
            avgPRefuse += statics.calcRejectedProbability(i);
            avgTSystem += statics.calcAvgInSystem(i);
        }
        avgPRefuse /= numSources;
        avgTSystem /= numSources;

        for (int d = 1; d <= nDevices; ++d)
        {
            avgUsage += statics.calcDeviceUsage(d);
        }
        avgUsage /= nDevices;

        results.push_back({ nDevices, avgPRefuse, avgTSystem, avgUsage });

        qDebug() << "Devices:" << nDevices
                 << " Refuse=" << avgPRefuse
                 << " Tsys=" << avgTSystem
                 << " Usage=" << avgUsage;
    }

    // построить графики
    buildChartsByDevices(results);
}
void SimulatorWindow::buildChartsByDevices(const std::vector<ExperimentResult>& results)
{
    using namespace QtCharts;

    std::vector<QPointF> pointsTSystem, pointsPRefuse, pointsUsage;
    for (const auto &r : results) {
        pointsTSystem.push_back(makePoint(r.xValue, r.tSystem));
        pointsPRefuse.push_back(makePoint(r.xValue, r.pRefuse));
        pointsUsage.push_back(makePoint(r.xValue, r.usage));
    }

    ui->chart1->setChart(createLineChart(
        "Вероятность отказа от количества приборов",
        "Количество приборов",
        "Вероятность отказа",
        pointsPRefuse));

    ui->chart2->setChart(createLineChart(
        "Среднее время пребывания в системе заявок от количества приборов",
        "Количество приборов",
        "Среднее время в системе",
        pointsTSystem));

    ui->chart3->setChart(createLineChart(
        "Коэффициент загрузки приборов от количества приборов",
        "Количество приборов",
        "Загрузка приборов",
        pointsUsage));
}

void SimulatorWindow::runExperimentsByBuffer()
{
    std::vector<ExperimentResult> results;

    const int numSources = 11;
    const int numDevices = 3;
    const double alpha = 5, beta = 7, lambda = 0.6;
    const int target = 4000;

    for (int bufferSize = 1; bufferSize <= 4; ++bufferSize)
    {
        Simulator sim(numSources, numDevices, bufferSize, alpha, beta, lambda, target);
        sim.init();
        sim.runAutoMode();
        sim.setStatisticsTimeTotalModeling(sim.getTime());
        const Statistics& statics = sim.getStatistics();

        double avgPRefuse = 0, avgTSystem = 0, avgUsage = 0;
        for (int i = 1; i <= numSources; ++i) {
            avgPRefuse += statics.calcRejectedProbability(i);
            avgTSystem += statics.calcAvgInSystem(i);
        }
        avgPRefuse /= numSources;
        avgTSystem /= numSources;

        for (int d = 1; d <= numDevices; ++d)
            avgUsage += statics.calcDeviceUsage(d);
        avgUsage /= numDevices;

        results.push_back({ bufferSize, avgPRefuse, avgTSystem, avgUsage });
    }

    buildChartsByBuffer(results);
}
void SimulatorWindow::buildChartsByBuffer(const std::vector<ExperimentResult>& results)
{
    using namespace QtCharts;

    std::vector<QPointF> pointsTSystem, pointsPRefuse, pointsUsage;
    for (const auto &r : results) {
        pointsTSystem.push_back(makePoint(r.xValue, r.tSystem));
        pointsPRefuse.push_back(makePoint(r.xValue, r.pRefuse));
        pointsUsage.push_back(makePoint(r.xValue, r.usage));
    }

    ui->chart4->setChart(createLineChart(
        "Вероятность отказа от количества мест в буфере",
        "Количество мест в буфере",
        "Вероятность отказа",
        pointsPRefuse));

    ui->chart5->setChart(createLineChart(
        "Среднее время в системе от количества мест в буфере",
        "Количество мест в буфере",
        "Среднее время в системе",
        pointsTSystem));

    ui->chart6->setChart(createLineChart(
        "Загруженность приборов от количества мест в буфере",
        "Количество мест в буфере",
        "Коэффициент загрузки",
        pointsUsage));
}

void SimulatorWindow::runExperimentsBySources()
{
    std::vector<ExperimentResult> results;

    const int numDevices = 3;
    const int bufferSize = 4;
    const double alpha = 5.0, beta = 7.0, lambda = 0.6;
    const int target = 4000;

    for (int nSources = 1; nSources <= 12; ++nSources)
    {
        Simulator sim(nSources, numDevices, bufferSize, alpha, beta, lambda, target);
        sim.init();
        sim.runAutoMode();
        sim.setStatisticsTimeTotalModeling(sim.getTime());
        const Statistics& st = sim.getStatistics();

        double avgPRefuse = 0, avgTSystem = 0, avgUsage = 0;
        for (int i = 1; i <= nSources; ++i) {
            avgPRefuse += st.calcRejectedProbability(i);
            avgTSystem += st.calcAvgInSystem(i);
        }
        avgPRefuse /= nSources;
        avgTSystem /= nSources;

        for (int d = 1; d <= numDevices; ++d)
            avgUsage += st.calcDeviceUsage(d);
        avgUsage /= numDevices;

        results.push_back({ nSources, avgPRefuse, avgTSystem, avgUsage });
    }

    buildChartsBySources(results);
}
void SimulatorWindow::buildChartsBySources(const std::vector<ExperimentResult>& results)
{
    using namespace QtCharts;

    std::vector<QPointF> pointsTSystem, pointsPRefuse, pointsUsage;
    for (const auto &r : results) {
        pointsTSystem.push_back(makePoint(r.xValue, r.tSystem));
        pointsPRefuse.push_back(makePoint(r.xValue, r.pRefuse));
        pointsUsage.push_back(makePoint(r.xValue, r.usage));
    }

    ui->chart7->setChart(createLineChart(
        "Вероятность отказа от количества источников",
        "Количество источников",
        "Вероятность отказа",
        pointsPRefuse));

    ui->chart8->setChart(createLineChart(
        "Среднее время в системе от количества источников",
        "Количество источников",
        "Среднее время в системе",
        pointsTSystem));

    ui->chart9->setChart(createLineChart(
        "Загруженность приборов от количества источников",
        "Количество источников",
        "Коэффициент\nзагрузки",
        pointsUsage));
}

int SimulatorWindow::estimateRequiredRequests(
    int numSources, int numDevices, int bufferSize,
    double alpha, double beta, double lambda)
{
    double confidenceLevel = 0.9;
    double delta = 0.1;

    const double t_alpha = 1.643; // для α = 0.9
    int N_current = 100; // начальное значение
    double p_prev = 0.0;
    double p_current = 0.0;
    bool firstIteration = true;

    qDebug() << "=== Оценка необходимого количества заявок ===";

    for (int iteration = 1; iteration <= 50; ++iteration)
    {
        Simulator sim(numSources, numDevices, bufferSize,
                      alpha, beta, lambda, N_current);
        sim.init();
        sim.runAutoMode();
        sim.setStatisticsTimeTotalModeling(sim.getTime());
        const Statistics &st = sim.getStatistics();

        // вычисляем среднюю вероятность отказа по всем источникам
        double p_sum = 0.0;
        for (int i = 1; i <= numSources; ++i)
            p_sum += st.calcRejectedProbability(i);
        p_current = p_sum / numSources;

        qDebug() << QString("Итерация %1: N=%2, p=%3")
                        .arg(iteration)
                        .arg(N_current)
                        .arg(p_current, 0, 'f', 5);

        if (!firstIteration)
        {
            double diff = std::abs(p_current - p_prev);
            if (diff < 0.1 * p_prev)
            {
                qDebug() << "Разница между p0 и p1 меньше 10%. Останавливаем.";
                break;
            }
        }

        firstIteration = false;
        p_prev = p_current;

        // вычисляем новое N
        if (p_current > 0.0 && p_current < 1.0)
        {
            double N_new = ((1 - p_current) * std::pow(t_alpha, 2)) / (p_current * std::pow(delta, 2));
            N_current = static_cast<int>(std::ceil(N_new));
        }
        else
        {
            qDebug() << "p вне диапазона (0,1), используем N=1000 по умолчанию.";
            p_current = 0.05;
            N_current = 1000;
        }
    }

    qDebug() << "Необходимое количество заявок для заданной точности:" << N_current;
    return N_current;
}
