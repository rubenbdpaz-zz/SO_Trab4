#include "meminfo.h"
#include <QHash>
//#include <QApplication>
#include <QtCore>
#include <unistd.h>
#include <iostream>
#include <QFile>


MEMinfo::MEMinfo()
{
    total = 0;
    free = 0;
}

/*MEMinfo::~MEMinfo()
{

}*/
bool MEMinfo::abrirArquivo(){
    QFile memFile("/proc/meminfo");
    if (memFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        fileInfo = memFile.readAll();
        memFile.close();
        return true;
    }
    else
        return false;
}


void MEMinfo::run(){
       //int pos;
       double value, swapValue;
       QString key, aux;
       QStringList fileData, attrib;
       QHash <QString, double> hash;
       
       while(true){
           fileData.clear();
           attrib.clear();
           hash.clear();
           if (abrirArquivo()){
               fileData = fileInfo.split("\n");
               for (int j = 0; j < fileData.size()-1; j++){
                   attrib = fileData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
                   key = attrib.at(0).simplified();
                   aux = attrib.at(1).simplified().remove("kB");
                   value = aux.toDouble();
                   hash.insert(key, value);
               }
               total = hash.value("MemTotal");
               free = hash.value("MemFree");
               swapTotal = hash.value("SwapTotal");
               //std::cout << swapTotal << std::endl;
               swapFree = hash.value("SwapFree");

               //std::cout << swapFree << std::endl;
               value = (total-free)*100/total;
               swapValue = (swapTotal - swapFree)*100/swapTotal;
               emit update(value, swapValue);
           }
           sleep(1);
    }
}

double MEMinfo::getTotal()
{
    return total;
}

double MEMinfo::getFree()
{
    return free;
}




