#ifndef MEMINFO_H
#define MEMINFO_H

#include "thread.h"
#include <QVector>
#include <QFile>

class MEMinfo: public Thread{
   Q_OBJECT

public:
    MEMinfo();
    //~MEMinfo();
    void run();
    double getTotal();
    double getFree();
    bool abrirArquivo();

signals:
    void update(QVector<double>, QVector<double>);

private:
   QVector<double> x, data;
   QFile memFile;
   QString fileInfo;
   double total, free;

};

#endif // MEMINFO_H
