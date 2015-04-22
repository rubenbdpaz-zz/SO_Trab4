#ifndef CPUINFO_H
#define CPUINFO_H

#include "thread.h"
#include <QStringList>
#include <QVector>

class CPUinfo : public Thread{
    Q_OBJECT

public:
    CPUinfo(int);
    //~CPUinfo();
    bool abrirArquivo();
    void run();
    void getClockTick();
    void setPercent();
    int getNumCPUS();
    int getCpuAtual();

private:
    QString fileInfo;
    int numCPU, cpuAtual, CLK_TCK;
    double t_uso, t_ocioso, t_boot;
    //QVector<double> delta1, delta2, delta3, delta4, percents;
    double delta1, delta2, delta3, delta4, percents;

signals:
    //void update(QVector<double>, double, double, double);
    void update(double, double, double, double, int);

};

#endif // CPUINFO_H
