#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    int numSources = ui->spinSources->value();
    int numDevices = ui->spinDevices->value();
    int bufferSize = ui->spinBufferSize->value();
    double alpha = ui->doubleAlpha->value();
    double beta = ui->doubleBeta->value();
    double lambda = ui->doubleLambda->value();
    int target = ui->spinTargetRequests->value();

    // создаём окно симулятора
    auto *simWin = new SimulatorWindow(numSources, numDevices, bufferSize,
                                       alpha, beta, lambda, target, this);
    simWin->show();
    this->hide();
}

void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}
