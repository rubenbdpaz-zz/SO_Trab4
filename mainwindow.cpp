#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDir>
#include <QFile>
#include <QHash>
#include <iostream>
#include "meminfo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ABA PROCESSOS
    model = new QStandardItemModel();
    QStringList headers;
    headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
    model->setHorizontalHeaderLabels(headers);

    procs = new Processos();

    connect(procs, SIGNAL(processInfo(QHash <QString, QString>)), SLOT (updateProcesses(QHash<QString, QString>)));
    procs->start();

    ui->Nprocs->display(procs->getNumProcessos());
    ui->NThreads->display(procs->getNumThreads());  //NÃO FUNCIONA

    //ABA INFORMAÇÕES DO SISTEMA

    //ICONE
    QPixmap icone("pinguim.jpg");
    ui->iconeSistema->setFixedWidth(64);
    ui->iconeSistema->setFixedHeight(64);
    ui->iconeSistema->setPixmap(icone);
    //ui->iconeSistema->show();

    //NOME DO COMPUTADOR
    QFile file("/proc/sys/kernel/hostname");
    QStringList fileData, attrib;
    QString HWinfo;
    QHash<QString, QString> hash;
    QString key, value, aux("Nome do Computador: ");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux += file.readLine();
        ui->NomePC->setText(aux);
    }
    file.close();

    //VERSÃO
    file.setFileName("/proc/version_signature");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux = file.readLine();
        ui->infoSO->setText(aux);
        ui->infoSO->adjustSize();
        //AJUSTAR QUEBRA DE LINHA
    }
    file.close();


    //INFORMAÇÕES DO COMPUTADOR
    file.setFileName("/proc/cpuinfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux = file.readAll();
        fileData = aux.split("\n");
        for (int j = 0; j < 5; j++){
            attrib = fileData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
            key = attrib.at(0).simplified();
            value = attrib.at(1).simplified();
            hash.insert(key, value);
        }
        HWinfo = hash.value("model name");
        HWinfo += "\n";
    }
    file.close();

    //INFORMAÇÕES DE MEMORIA // MUDAR PRA PEGAR AS INFORMAÇÕES DA THREAD DE MEMORIA
    float memSize;
    file.setFileName("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux = file.readAll();
        fileData = aux.split("\n");
        for (int j = 0; j < 2; j++){
            attrib = fileData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
            key = attrib.at(0).simplified();
            value = attrib.at(1).simplified();
            value.remove("kB");
            memSize = value.toFloat();
            memSize /= (1024*1024);
            value.setNum(memSize,'f', 1);
            value.append(" GiB");
            hash.insert(key, value);
        }
        HWinfo += "Memoria: " + hash.value("MemTotal") + " (" + hash.value("MemFree") + " livre)";
        ui->infoHW->setText(HWinfo);
    }
    file.close();


    //ABA DESEMPENHO

    //GRÁFICO DA CPU
    ui->cpuGraph->yAxis->setRange(0, 100);
    ui->cpuGraph->yAxis->setAutoTickStep(false);
    ui->cpuGraph->yAxis->setTickStep(50);

    //GRÁFICO DA MEMÓRIA
    threadMem = new MEMinfo();
    setMemoryGraph();

    connect(threadMem, SIGNAL(update(QVector<double>, QVector<double>)), SLOT(updateMemoryGraph(QVector<double>, QVector<double>)));

    threadMem->start();
 }

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setMemoryGraph(){
    ui->memoryGraph->yAxis->setRange(0, 100);
    ui->memoryGraph->yAxis->setAutoTickStep(false);
    ui->memoryGraph->yAxis->setTickStep(50);
    ui->memoryGraph->xAxis->setRange(0, 60);
    ui->memoryGraph->addGraph();
    ui->memoryGraph->graph(0)->setName("Uso de Memória");
    ui->memoryGraph->legend->setVisible(true);
    ui->memoryGraph->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignLeft|Qt::AlignBottom);


}

void MainWindow::updateMemoryGraph(QVector<double> x, QVector<double> values){
    ui->memoryGraph->graph(0)->setData(x, values);
}

void MainWindow::updateProcesses(QHash<QString, QString> newHash){
   listaItem << new QStandardItem(newHash.value("Name"));
   listaItem << new QStandardItem(newHash.value("State"));
   listaItem << new QStandardItem(newHash.value("Pid"));
   listaItem << new QStandardItem(newHash.value("PPid"));
   listaItem << new QStandardItem(newHash.value("Uid"));   // CONVERTER PARA O NOME DO USUÁRIO: system("getent passwd " + hash.value("Uid"))
   listaItem << new QStandardItem(newHash.value("Threads"));
   listaItem << new QStandardItem(newHash.value("ContextSwitches"));
   model->appendRow(listaItem);

  //model->appendRow(listaItem);

  ui->tableView->setModel(model);
  ui->tableView->setShowGrid(false);
  ui->tableView->setAlternatingRowColors(true);
  ui->tableView->verticalHeader()->setVisible(false);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->sortByColumn(4, Qt::AscendingOrder);

}

void MainWindow::on_pushButton_clicked()
{
    //listaitem. (....) ?
}
