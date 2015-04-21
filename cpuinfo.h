#ifndef CPUINFO_H
#define CPUINFO_H

#include "thread.h"
#include <QStringList>
#include <QVector>

class CPUinfo : public Thread{
    Q_OBJECT

public:
    CPUinfo();
    //~CPUinfo();
    bool abrirArquivo();
    void run();
    void initDeltas();
    void setPercent();
    int getNumCPUS();

private:
    QString fileInfo;
    int numCPU;
    QVector<double> delta1, delta2, delta3, delta4, percents;

signals:
    void update(QVector<double>);

};

#endif // CPUINFO_H
