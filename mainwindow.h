#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QSplitter>
#include "meminfo.h"
#include "processos.h"
#include "cpuinfo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setMemoryGraph();
    //void updateMemoryGraph(QVector<double>, QVector<double>);
    void updateMemoryGraph(double);
    void updateCPU(QVector<double>);

    void updateProcesses(QHash<QString, QString>);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QSplitter *splitter;
    //Thread *thread1, *thread2;
    MEMinfo *threadMem;
    Processos *procs;
    CPUinfo *threadCPU;
    QList<QStandardItem *> listaItem;
    QVector<double> x, memData;

};

#endif // MAINWINDOW_H
