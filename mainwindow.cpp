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

  QDir proc("/proc");
  QStringList dirList, filters, statusData, attrib;
  QString key, value, aux, line;
  QHash<QString, QString> hash; //Dicionário
  QFile process_Status;
  int i = 0, j;

  //PEGANDO TODAS AS PASTAS DENTRO DE /PROC CUJO NOME SÃO NÚMEROS
  filters << "*[0-9]";  //FILTRO DE NOME DA PASTA
  proc.setNameFilters(filters);
  proc.setFilter(QDir::Dirs); //FILTRO PARA PEGAR APENAS PASTAS
  dirList = proc.entryList(); //Lista com o nome de todos os diretórios cujo nome são números

  QList<QStandardItem *> listaItem;

  //ENTRANDO EM CADA PASTA DE PROCESSO E PEGANDO OS DADOS DO ARQUIVO STATUS
  for (i = 0; i < dirList.size(); i++){
      proc.cd(dirList.at(i));    //ENTRANDO NO DIRETORIO
      process_Status.setFileName(proc.absoluteFilePath("status"));    //ABRINDO O ARQUIVO STATUS
      if (process_Status.open(QIODevice::ReadOnly | QIODevice::Text)){
             line = process_Status.readAll(); //PEGANDO O CONTEÚDO DO ARQUIVO STATUS
             statusData = line.split("\n");          //Separando cada linha de "status" e criando uma lista com cada linha;
             for (j = 0; j < statusData.size()-1; j++){
                 attrib = statusData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
                 key = attrib.at(0).trimmed();
                 value = attrib.at(1).trimmed();
                 hash.insert(key, value);

            }
             int trContexto = hash.value("voluntary_ctxt_switches").toInt() + hash.value("nonvoluntary_ctxt_switches").toInt();
             aux.setNum(trContexto);
             //hash.value("Uid") =
             listaItem << new QStandardItem(hash.value("Name"));
             listaItem << new QStandardItem(hash.value("State"));
             listaItem << new QStandardItem(hash.value("Pid"));
             listaItem << new QStandardItem(hash.value("PPid"));
             listaItem << new QStandardItem(hash.value("Uid"));   // CONVERTER PARA O NOME DO USUÁRIO: system("getent passwd " + hash.value("Uid"))
             listaItem << new QStandardItem(hash.value("Threads"));
             listaItem << new QStandardItem(aux);

             model->appendRow(listaItem);
             listaItem.clear();
             hash.clear();
             statusData.clear();
             attrib.clear();

     }
     process_Status.close();
     proc.cdUp(); // Voltando para o diretório pai (/proc)
 }
    model->appendRow(listaItem);

    ui->tableView->setModel(model);
    ui->tableView->setShowGrid(false);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(4, Qt::AscendingOrder);



    //ABA INFORMAÇÕES DO SISTEMA

    //ICONE
    QPixmap icone("pinguim.jpg");
    ui->iconeSistema->setFixedWidth(64);
    ui->iconeSistema->setFixedHeight(64);
    ui->iconeSistema->setPixmap(icone);
    ui->iconeSistema->show();

    //NOME DO COMPUTADOR
    QFile file("/proc/sys/kernel/hostname");
    QStringList fileData;
    QString HWinfo;
    aux =  "Nome do Computador: ";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux += file.readLine();
        ui->NomePC->setText(aux);
    }
    file.close();

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
        for (j = 0; j < 5; j++){
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
        for (j = 0; j < 2; j++){
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
/*

    QString cpuData[2];
    QStringList cpuDataLines, cpuDataLinesInfo;
    QFile cpuInfo("/proc/stat");
    int NumProcessors;
    int count;

    if (cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text)){
        cpuData[0] = cpuInfo.readAll();
    }
    cpuInfo.close();

    sleep(1);

    if (cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text)){
        cpuData[1] = cpuInfo.readAll();
    }
    cpuInfo.close();

    NumProcessors = cpuData.count("cpu") - 1;
    cpuDataLines = cpuData.split("\n");
    for (count = 0; count <= NumProcessors; count++){
        cpuDataLinesInfo = cpuDataLines[i].split(" ");
    }

*/

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
}

void MainWindow::updateMemoryGraph(QVector<double> x, QVector<double> values){
    ui->memoryGraph->graph(0)->setData(x, values);
}
