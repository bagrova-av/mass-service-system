#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include <QDialog>

#include "../app/statistics.h"


namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsDialog(const Statistics& stats, double totalTime, QWidget *parent = nullptr);
    ~ResultsDialog();

private slots:
    void on_closeResultsButton_clicked();

private:
    Ui::ResultsDialog *ui;
    const Statistics& statisticsRef;  // ссылка на статистику
    double totalModelingTime;      // общее время моделирования

    void setupSourcesTable();
    void setupDevicesTable();
};

#endif // RESULTSDIALOG_H
