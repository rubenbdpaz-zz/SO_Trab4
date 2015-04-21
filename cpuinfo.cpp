#include "cpuinfo.h"
#include <QFile>
#include <iostream>
#include <unistd.h>

CPUinfo::CPUinfo()
{
    abrirArquivo();
    numCPU = fileInfo.count("cpu") - 1;
    delta1.resize(numCPU);
    delta2.resize(numCPU);
    delta3.resize(numCPU);
    delta4.resize(numCPU);
    initDeltas();
    percents.resize(numCPU);
    percents.fill(0);
}

/*CPUinfo::~CPUinfo()
{

}
*/

void CPUinfo::initDeltas(){
    delta1.fill(0);
    delta2.fill(0);
    delta3.fill(0);
    delta4.fill(0);
}

void CPUinfo::setPercent(){
    for (int i = 0; i < numCPU; i++){
        percents[i] = delta1[i] + delta2[i] + delta3[i];
        percents[i] = percents[i]*100/(percents[i]+delta4[i]);
    }
}

bool CPUinfo::abrirArquivo(){
    QFile cpuFile("/proc/stat");
    if (cpuFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        fileInfo = cpuFile.readAll();
        cpuFile.close();
        return true;
    }
    else
        return false;
}


void CPUinfo::run(){
    QStringList fileLines, lineData;
    while (true){
        for (int j = 0; j < 2; j++){
            if (abrirArquivo()){
                fileLines = fileInfo.split("\n");
                for (int i = 0; i < numCPU; i++){
                    lineData = fileLines[i+1].split(QRegExp("\\s+"));
                    delta1[i] = lineData[1].toDouble() - delta1[i];
                    delta2[i] = lineData[2].toDouble() - delta2[i];
                    delta3[i] = lineData[3].toDouble() - delta3[i];
                    delta4[i] = lineData[4].toDouble() - delta4[i];
                }
            }
            sleep(1);
        }
        setPercent();
        emit(update(percents));
    }
}

int CPUinfo::getNumCPUS(){
    return numCPU;
}
