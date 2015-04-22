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

    qRegisterMetaType<QHash <QString, QString> >("QHash <QString, QString>");
    connect(procs, SIGNAL(processInfo(QHash <QString, QString>)), SLOT (updateProcesses(QHash<QString, QString>)));
    procs->start();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerSlot()));

    timer->start(2000);

    //ABA DESEMPENHO

    x.resize(60);
    for (int i = 60, j = 0; i > 0; i--, j++)
        x[j] = j;

    //GRÁFICO DA CPU
    threadCPU = new CPUinfo(-1);
    threadCPU->start();
    setCPUgraph();

    for (int i = 0; i < threadCPU->getNumCPUS(); i++){
        CPUinfo *cpu = new CPUinfo(i);
        cpu->start();
        qRegisterMetaType<QVector<double> >("QVector<double>");
        connect(cpu, SIGNAL(update(double, double, double, double, int)), SLOT(updateCPU(double,double,double,double, int)));
        //connect(threadCPU, SIGNAL(update(QVector<double>, double, double, double)), SLOT(updateCPU(QVector<double>, double, double, double)));
    }

    //GRÁFICO DA MEMÓRIA

    memData.resize(60);
    memData.fill(0);
    threadMem = new MEMinfo();
    setMemoryGraph();

    //connect(threadMem, SIGNAL(update(QVector<double>, QVector<double>)), SLOT(updateMemoryGraph(QVector<double>, QVector<double>)));
    connect(threadMem, SIGNAL(update(double)), SLOT(updateMemoryGraph(double)));
    threadMem->start();
    //threadMem->run();

    //ABA INFORMAÇÕES DO SISTEMA

    //ICONE
    QPixmap icone("/usr/share/icons/default/pinguim.jpg");
    ui->iconeSistema->setPixmap(icone);

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

   //INFORMAÇÕES DE MEMORIA
    double memSize;
    memSize = (threadMem->getTotal())/(1024*1024);
    HWinfo += "Memoria: " + value.setNum(memSize, 'f', 1) + " GB (";
    memSize = (threadMem->getFree())/(1024*1024);
    HWinfo += value.setNum(memSize, 'f', 1) + " GB livres )";
    ui->infoHW->setText(HWinfo);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setMemoryGraph(){
    ui->memoryGraph->yAxis->setRange(0, 100);
    ui->memoryGraph->yAxis->setAutoTickStep(false);
    ui->memoryGraph->yAxis->setTickStep(50);
    ui->memoryGraph->xAxis->setRange(60, 0);
    ui->memoryGraph->xAxis->setTickStep(15);
    ui->memoryGraph->addGraph();
    ui->memoryGraph->graph(0)->setName("Uso de Memória");
    //ui->memoryGraph->legend->setVisible(true);
    //ui->memoryGraph->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignLeft|Qt::AlignBottom);


}

void MainWindow::updateMemoryGraph(double latestData){
    double value;
    for (int pos = 0; pos < 59; ++pos){
        value = memData[pos+1];
        memData.replace(pos, value);
    }
    memData[59] = latestData;
    ui->memoryGraph->graph(0)->setData(x, memData);
    ui->memoryGraph->replot();
}

void MainWindow::updateProcesses(QHash<QString, QString> newHash){
   listaItem.clear();
   listaItem << new QStandardItem(newHash.value("Name"));
   listaItem << new QStandardItem(newHash.value("State"));
   listaItem << new QStandardItem(newHash.value("Pid"));
   listaItem << new QStandardItem(newHash.value("PPid"));
   listaItem << new QStandardItem(newHash.value("Uid"));   // CONVERTER PARA O NOME DO USUÁRIO: system("getent passwd " + hash.value("Uid"))
   listaItem << new QStandardItem(newHash.value("Threads"));
   listaItem << new QStandardItem(newHash.value("ContextSwitches"));
   model->appendRow(listaItem);

  ui->tableView->setModel(model);
  ui->tableView->setShowGrid(false);
  ui->tableView->setAlternatingRowColors(true);
  ui->tableView->verticalHeader()->setVisible(false);
  ui->tableView->setSortingEnabled(true);
  ui->tableView->sortByColumn(4, Qt::AscendingOrder);

  ui->Nprocs->display(procs->getNumProcessos());
  ui->NThreads->display(procs->getNumThreads());

}

void MainWindow::setCPUgraph(){
    int nCPU = threadCPU->getNumCPUS();
//    cpuData.resize(nCPU);
    for(int i = 0; i < nCPU; i++){
        //cpuData[i].resize(60);
        //cpuData[i].fill(0);
        ui->cpuGraph->addGraph();
    }
    //std::cout << nCPU << std::endl;
    cpuData.resize(60);
    cpuData.fill(0);
    ui->cpuGraph->yAxis->setRange(0, 100);
    ui->cpuGraph->yAxis->setAutoTickStep(false);
    ui->cpuGraph->yAxis->setTickStep(50);
    ui->cpuGraph->xAxis->setRange(60, 0);
    ui->cpuGraph->xAxis->setTickStep(15);
}

//void MainWindow::updateCPU(QVector<double> percent, double uso, double ocioso, double boot){  //VETOR DE CPUS?!?!
void MainWindow::updateCPU(double percent, double uso, double ocioso, double boot, int cpuAtual){  //VETOR DE CPUS?!?!
    QString currentCPU, name = "CPU ";
    ui->usoCPU->setText(currentCPU.setNum(uso) + " s");
    ui->ocioso->setText(currentCPU.setNum(ocioso) + " s");
    ui->uptime->setText(currentCPU.setNum(boot) + " s");
    double value;
/*    //int i = 0;
    for (int i = 0; i < threadCPU->getNumCPUS(); i++){
        //std::cout <<"value: " << value << std::endl;
        for (int pos = 0; pos < 59; ++pos){
            value = cpuData[i][pos+1];
            cpuData[i].replace(pos, value);
        }
        cpuData[i][59] = percent[i];
        //std::cout << cpuData[i][59] << std::endl;
        ui->cpuGraph->addGraph();
        currentCPU.setNum(i+1);
        name += currentCPU;
        ui->cpuGraph->graph(i)->setName(name);
*/
    for (int pos = 0; pos < 59; ++pos){
        value = cpuData[pos+1];
        cpuData.replace(pos, value);
    }
    cpuData[59] = percent;
    ui->cpuGraph->graph(cpuAtual)->setData(x, cpuData);
    ui->cpuGraph->replot();
        //ESCOLHENDO A COR
        switch (cpuAtual){
        case 0:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::blue));
            break;
        case 1:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::red));
            break;
        case 2:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::green));
            break;
        case 3:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::yellow));
            break;
        case 4:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::black));
            break;
        case 5:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::cyan));
            break;
        case 6:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::magenta));
            break;
        case 7:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::gray));
            break;
        case 8:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::darkRed));
            break;
        }
        ui->cpuGraph->graph(cpuAtual)->setData(x, cpuData);
        ui->cpuGraph->replot();
}

void MainWindow::on_pushButton_clicked()
{
    //listaitem. (....) ?
    //ui->tableView->selectRow(); (???)
    //ver help do QtableView
}

void MainWindow::TimerSlot(){
    model->clear();
    QStringList headers;
    headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
    model->setHorizontalHeaderLabels(headers);
    procs->run();
}
