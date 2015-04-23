#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDir>
#include <QFile>
#include <QHash>
#include <iostream>
#include "meminfo.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->atualizarDial->hide();
    ui->label_2->hide();

    //ui->pushButton->hide();
    //ABA PROCESSOS
    model = new QStandardItemModel();
    QStringList headers;
    headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
    model->setHorizontalHeaderLabels(headers);

    ui->atualizarDial->setRange(3, 10);
    ui->atualizarDial->setTracking(false);

    procs = new Processos();

    qRegisterMetaType<QHash <QString, QString> >("QHash <QString, QString>");
    connect(procs, SIGNAL(processInfo(QHash <QString, QString>)), SLOT (updateProcesses(QHash<QString, QString>)));
    procs->start();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerSlot()));
    //connect(ui->atualizarDial, SIGNAL(valueChanged(int)), timer, SLOT(start(int)));
    timer->setInterval(60000);
    timer->start();

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
    swapData.resize(60);
    swapData.fill(0);
    threadMem = new MEMinfo();
    setMemoryGraph();

    //connect(threadMem, SIGNAL(update(QVector<double>, QVector<double>)), SLOT(updateMemoryGraph(QVector<double>, QVector<double>)));
    connect(threadMem, SIGNAL(update(double, double)), SLOT(updateMemoryGraph(double, double)));
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
    ui->memoryGraph->addGraph();
    ui->memoryGraph->graph(1)->setName("Swap");
    ui->memoryGraph->graph(1)->setPen(QPen(Qt::red));
    ui->memoryGraph->legend->setVisible(true);
    ui->memoryGraph->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignLeft|Qt::AlignBottom);


}

void MainWindow::updateMemoryGraph(double latestData, double latestSwap){
    double value;
    for (int pos = 0; pos < 59; ++pos){
        value = memData[pos+1];
        memData.replace(pos, value);
        value = swapData[pos+1];
        swapData.replace(pos, value);
    }
    memData[59] = latestData;
    swapData[59] = latestSwap;
    ui->memoryGraph->graph(0)->setData(x, memData);
    ui->memoryGraph->graph(1)->setData(x, swapData);
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
    ui->cpuGraph->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignLeft|Qt::AlignBottom);
    ui->cpuGraph->legend->setVisible(true);

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
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 1");
            break;
        case 1:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::red));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 2");
            break;
        case 2:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::green));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 3");
            break;
        case 3:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::yellow));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 4");
            break;
        case 4:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::black));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 5");
            break;
        case 5:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::cyan));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 6");
            break;
        case 6:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::magenta));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 7");
            break;
        case 7:
            ui->cpuGraph->graph(cpuAtual)->setPen(QPen(Qt::gray));
            ui->cpuGraph->graph(cpuAtual)->setName("CPU 8");
            break;
        }
        ui->cpuGraph->graph(cpuAtual)->setData(x, cpuData);
        ui->cpuGraph->replot();
}

void MainWindow::on_pushButton_clicked()
{
    QString pid, command("kill -9 ");
    //QModelIndex index;
    //QTableWidgetItem item;
    int row, code;

    row = ui->tableView->selectionModel()->currentIndex().row();
    //std::cout << row << std::endl;
    if (row == -1){
        QMessageBox::information(this, tr("Erro"), tr("Por favor, tente novamente"));
    }
    else{
        pid = ui->tableView->model()->data(ui->tableView->model()->index(row, 2)).toString();
        //std::cout << pid.toStdString() << std::endl;
        command += pid;
        code = system(qPrintable(command));
        if (code == 0)
            QMessageBox::information(this, tr("Erro"), tr("Processo finalizado com sucesso"));
        else
            QMessageBox::information(this, tr("Erro"), tr("Erro ao finalizar o processo"));
        procs->run();
    }
}


void MainWindow::TimerSlot(){
    model->clear();
    QStringList headers;
    headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
    model->setHorizontalHeaderLabels(headers);
    procs->run();
}

void MainWindow::on_atualizarDial_valueChanged(int value)
{
    //value *= 1000;
    //timer->stop();
    //std::cout << value << std::endl;

    //timer->setInterval(value*1000);
    //timer->start();
}

void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    ui->pushButton->show();
}

void MainWindow::on_atualizar_clicked()
{
    model->clear();
    QStringList headers;
    headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
    model->setHorizontalHeaderLabels(headers);
    procs->run();
}
