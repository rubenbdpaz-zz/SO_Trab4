#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QSplitter>
#include "meminfo.h"
#include "processos.h"

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
    void updateMemoryGraph(QVector<double>, QVector<double>);
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
    QList<QStandardItem *> listaItem;

};

#endif // MAINWINDOW_H
