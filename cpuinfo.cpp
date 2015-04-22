#include "cpuinfo.h"
#include <QFile>
#include <iostream>
#include <unistd.h>

CPUinfo::CPUinfo(int i)
{
    cpuAtual = i;
    abrirArquivo();
    numCPU = fileInfo.count("cpu") - 1;
/*    delta1.resize(numCPU+1);
    delta2.resize(numCPU+1);
    delta3.resize(numCPU+1);
    delta4.resize(numCPU+1);
    initDeltas();
    percents.resize(numCPU);
    percents.fill(0);
    */
    delta1 = delta2 = delta3 = delta4 = percents = 0;
    getClockTick();


}

/*CPUinfo::~CPUinfo()
{
ncpus
QVector cpuatual
}
*/


void CPUinfo::getClockTick(){
    QString clockTick;
    QFile aux("clocktick.txt");

    system("getconf CLK_TCK > clocktick.txt");
    if (aux.open(QIODevice::ReadOnly | QIODevice::Text))
            clockTick = aux.readLine();
    aux.close();
    CLK_TCK = clockTick.toInt();
}

void CPUinfo::setPercent(){
    /*for (int i = 0; i < numCPU; i++){
        percents[i] = delta1[i+1] + delta2[i+1] + delta3[i+1];
        percents[i] = percents[i]*100/(percents[i]+delta4[i+1]);
    }*/
    percents = delta1 + delta2 + delta3;
    percents = percents*100/(percents + delta4);
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
/*  QStringList fileLines, lineData;
    while (true){
        //int clocktck = system("getconf CLK_TCK");
        for (int j = 0; j < 2; j++){
            if (abrirArquivo()){
                fileLines = fileInfo.split("\n");
                for (int i = 0; i <= numCPU; i++){
                    lineData = fileLines[i].split(QRegExp("\\s+"));
                    if (i == 0){
                        delta1[0] = lineData[1].toDouble();
                        delta2[0] = lineData[2].toDouble();
                        delta3[0] = lineData[3].toDouble();
                        delta4[0] = lineData[4].toDouble();
                    }
                    else{
                        delta1[i] = lineData[1].toDouble() - delta1[i];
                        delta2[i] = lineData[2].toDouble() - delta2[i];
                        delta3[i] = lineData[3].toDouble() - delta3[i];
                        delta4[i] = lineData[4].toDouble() - delta4[i];
                    }
                }
            }
            sleep(1);
        }

        setPercent();
        t_uso = (delta1[0] + delta2[0] + delta3[0])/100;
        t_ocioso = delta4[0]/100;
        t_boot = (t_uso + t_ocioso);
        emit(update(percents, t_uso, t_ocioso, t_boot));
    }
*/
    QStringList fileLines, lineData;
        while (true){
            //int clocktck = system("getconf CLK_TCK");
            for (int j = 0; j < 2; j++){
                if (abrirArquivo()){
                    fileLines = fileInfo.split("\n");
                    lineData = fileLines[cpuAtual+1].split(QRegExp("\\s+"));
                    delta1 = lineData[1].toDouble() - delta1;
                    delta2 = lineData[2].toDouble() - delta2;
                    delta3 = lineData[3].toDouble() - delta3;
                    delta4 = lineData[4].toDouble() - delta4;
                 }
                sleep(1);
            }
            lineData = fileLines[0].split(QRegExp("\\s+"));
            t_uso = (lineData[1].toDouble() + lineData[2].toDouble() + lineData[3].toDouble())/CLK_TCK;
            t_ocioso = lineData[4].toDouble()/CLK_TCK;
            t_boot = t_uso + t_ocioso;

            setPercent();
            emit(update(percents, t_uso, t_ocioso, t_boot, cpuAtual));
        }
}

int CPUinfo::getNumCPUS(){
    return numCPU;
}
