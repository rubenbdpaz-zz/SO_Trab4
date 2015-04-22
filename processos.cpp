#include "processos.h"
#include <QtCore>

Processos::Processos()
{
    start();
    dir.setPath("/proc");
    numThreads = 0;
    setProcList();
    numProcessos = procList.count();
/*
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerSlot()));

    timer->start(10000);*/
}

/*void Processos::TimerSlot()
{
    this->run();
}
*/
Processos::~Processos()
{

}

void Processos::setProcList(){
    QStringList filters("*[0-9]");
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Dirs);
    procList = dir.entryList();
}

bool Processos::abrirArquivo(QString fileName){
    bool test;
    dir.cd(fileName);
    statusFile.setFileName(dir.absoluteFilePath("status"));
    if (statusFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        fileInfo = statusFile.readAll();
        dir.cdUp();
        test = true;
    }
    else
        test = false;
    statusFile.close();
    return test;
}

void Processos::run(){
    //ENTRANDO EM CADA PASTA DE PROCESSO E PEGANDO OS DADOS DO ARQUIVO STATUS

    QStringList statusLines, attrib;
    QString key, value;
    for (int i = 0; i < procList.size(); i++){
        statusLines.clear();
        attrib.clear();
        hash.clear();
        if (abrirArquivo(procList.at(i))){
               statusLines = fileInfo.split("\n");          //Separando cada linha de "status" e criando uma lista com cada linha;
               for (int j = 0; j < statusLines.size()-1; j++){
                   attrib = statusLines.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
                   key = attrib.at(0).trimmed();
                   value = attrib.at(1).trimmed();
                   hash.insert(key, value);
               }
        }
        contextSwitches = hash.value("voluntary_ctxt_switches").toInt() + hash.value("nonvoluntary_ctxt_switches").toInt();
        value.setNum(contextSwitches);
        hash.insert("ContextSwitches", value);
        numThreads += hash.value("Threads").toInt();
        emit processInfo(hash);

      }

}

int Processos::getNumProcessos(){
    return numProcessos;
}

int Processos::getNumThreads(){
    return numThreads;
}
