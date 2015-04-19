#include "meminfo.h"
#include <QHash>
//#include <QApplication>
#include <QtCore>
#include <unistd.h>


MEMinfo::MEMinfo()
{
    x.resize(60);
    for (int i = 0; i < 60; i++)
        x[i] = i;
    data.resize(60);
    data.fill(0);
    total = 0;
    free = 0;
}

/*MEMinfo::~MEMinfo()
{

}*/
bool MEMinfo::abrirArquivo(){
    bool test;
    memFile.setFileName("/proc/meminfo");
    if (memFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        fileInfo = memFile.readAll();
        test = true;
    }
    else{
        test = false;
    }
    memFile.close();
    return test;
}


void MEMinfo::run(){
       int pos;
       double value;
       QString aux;
       QStringList fileData, attrib;
       QHash <QString, double> hash;
       
       while(true){
           emit update(x, data);
           if (abrirArquivo()){
               fileData = fileInfo.split("\n");
               for (int j = 0; j < 2; j++){
                   attrib = fileData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
                   aux = attrib.at(0).simplified();
                   attrib.at(1).simplified().remove("kB");
                   value = attrib.at(1).toDouble();
                   hash.insert(aux, value);
               }
               for (pos = 0; pos < 59; ++pos){
                   value = data[pos+1];
                   data.replace(pos, value);
               }
               total = hash.value("MemTotal");
               free = hash.value("MemFree");
               data[59] = (total-free)*100/total;
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




