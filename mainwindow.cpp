#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDir>
#include <QFile>
#include <QHash>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


  //CRIAR THREAD PARA GERENCIAR AS 3 ABAS DE UMA VEZ
  model = new QStandardItemModel();
  QStringList headers;
  headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
  model->setHorizontalHeaderLabels(headers);

// model->setHorizontalHeaderItem( 0, new QStandardItem( "Nome" ) );
// model->setHorizontalHeaderItem( 1, new QStandardItem( "Status" ) );
 // model->setHorizontalHeaderItem( 2, new QStandardItem( "PID" ) );

  QDir proc("/proc");
  QStringList dirList, filters, statusData, attrib;
  QString dirAtual, statusPath, key, value, aux;
  QHash<QString, QString> hash; //Dicionário
  int i = 0, j;

  //PEGANDO TODAS AS PASTAS DENTRO DE /PROC CUJO NOME SÃO NÚMEROS
  filters << "*[0-9]";
  proc.setNameFilters(filters);
  proc.setFilter(QDir::Dirs);
  dirList = proc.entryList(); //Lista com o nome de todos os diretórios cujo nome são números

  //ENTRANDO EM CADA DIRETÓRIO E PEGANDO OS DADOS DO ARQUIVO STATUS

  QList<QStandardItem *> listaItem;
  for (i = 0; i < dirList.size(); i++){
      dirAtual = dirList.at(i);
      proc.cd(dirAtual);    //ENTRANDO NO DIRETORIO
      statusPath = proc.absoluteFilePath("status");   //ENDEREÇO DO ARQUIVO STATUS
      QFile process_Status(statusPath);    //ABRINDO O ARQUIVO STATUS
      if (process_Status.open(QIODevice::ReadOnly | QIODevice::Text)){
             QString line = process_Status.readAll(); //PEGANDO O CONTEÚDO DO ARQUIVO STATUS
             statusData = line.split("\n");          //Separando cada linha de "status" e criando uma lista com cada linha;
             for (j = 0; j < statusData.size()-1; j++){
                 attrib = statusData.at(j).split(":"); //SEPARANDO IDENTIFICADOR E DADO
                 key = attrib.at(0).trimmed();
                 value = attrib.at(1).trimmed();
                 hash.insert(key, value);
             }
             int trContexto = hash.value("voluntary_ctxt_switches").toInt() + hash.value("nonvoluntary_ctxt_switches").toInt();
             aux.setNum(trContexto);
             listaItem << new QStandardItem(hash.value("Name"));
             listaItem << new QStandardItem(hash.value("State"));
             listaItem << new QStandardItem(hash.value("Pid"));
             listaItem << new QStandardItem(hash.value("PPid"));
             listaItem << new QStandardItem(hash.value("Uid"));   // CONVERTER PARA O NOME DO USUÁRIO
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

 /*   listaItem << new QStandardItem("word");
    listaItem << new QStandardItem("Executando");
    listaItem << new QStandardItem("345");
    listaItem << new QStandardItem("1");
    listaItem << new QStandardItem("IMD");
    listaItem << new QStandardItem("6");
    listaItem << new QStandardItem("12345");
*/
    model->appendRow(listaItem);

    ui->tableView->setModel(model);
    ui->tableView->setShowGrid(false);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(4, Qt::AscendingOrder);

    //ESSA PARTE DEVE FICAR DENTRO DE UMA THREAD?

    QPixmap icone("/usr/share/icons/LoginIcons/apps/64/computer.svg");
    ui->iconeSistema->setFixedWidth(64);
    ui->iconeSistema->setFixedHeight(64);
    ui->iconeSistema->setPixmap(icone);
    //ui->iconeSistema->show();

    QFile file("/proc/sys/kernel/hostname");
    QStringList fileData;
    QString HWinfo;
    aux =  "Nome do Computador: ";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux += file.readLine();
        ui->NomePC->setText(aux);
    }
    file.close();

    file.setFileName("/proc/version");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        aux = file.readLine();
        ui->infoSO->setText(aux);
        ui->infoSO->adjustSize();
        //AJUSTAR QUEBRA DE LINHA
    }
    file.close();

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
            //value.setNum(memSize);
            value.append(" GiB");
            hash.insert(key, value);
        }
        HWinfo += "Memoria: " + hash.value("MemTotal") + " (" + hash.value("MemFree") + " livre)";
        ui->infoHW->setText(HWinfo);
    }
    file.close();

 }

MainWindow::~MainWindow()
{
    delete ui;
}
