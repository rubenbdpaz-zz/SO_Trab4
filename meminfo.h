#ifndef MEMINFO_H
#define MEMINFO_H

#include "thread.h"

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
    void update(double, double);
private:
   QString fileInfo;
   double total, free, swapTotal, swapFree;

};

#endif // MEMINFO_H
