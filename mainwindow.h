#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QSplitter>
#include "meminfo.h"
#include "processos.h"
#include "cpuinfo.h"
#include <QVector>

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
    void setCPUgraph();
    //void updateMemoryGraph(QVector<double>, QVector<double>);
    void updateMemoryGraph(double, double);
    //void updateCPU(QVector<double>, double, double, double);
    void updateCPU(double, double, double, double, int);
    void updateProcesses(QHash<QString, QString>);
    void TimerSlot();

private slots:
    void on_pushButton_clicked();

    void on_atualizarDial_valueChanged(int value);

    void on_tableView_activated(const QModelIndex &index);

    void on_atualizar_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QSplitter *splitter;
    //Thread *thread1, *thread2;
    MEMinfo *threadMem;
    Processos *procs;
    CPUinfo *threadCPU;
    QList<QStandardItem *> listaItem;
    QVector<double> x, memData, cpuData, swapData;
    //QVector< QVector<double> > cpuData;
    QTimer *timer;

};

#endif // MAINWINDOW_H
