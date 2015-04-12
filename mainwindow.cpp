#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

  model = new QStandardItemModel();
  QStringList headers;
  headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
  model->setHorizontalHeaderLabels(headers);
  //PEGAR INFORMAÇÕES DA PASTA PROC
  //CONTADOR DE QUANTAS PASTAS COM NUMERO TEM
  //FOR ATE ESSE NUMERO DE PASTAS
  //ENTRAR EM CADA PASTA, PEGAR AS INFORMAÇÕES DO STATUS E ENVIAR PRO LISTAITEM

// model->setHorizontalHeaderItem( 0, new QStandardItem( "Nome" ) );
// model->setHorizontalHeaderItem( 1, new QStandardItem( "Status" ) );
 // model->setHorizontalHeaderItem( 2, new QStandardItem( "PID" ) );

    /* PEGAR LISTA DE PROCESSOS NA PASTA DIR
    QDir proc("/proc");
    QStringList filters;
    filters << " ";
    proc.setNameFilters(filters);
    proc.setFilter(QDir::Dirs);

    QStringList dirList = proc.entryList();
    */
    QList<QStandardItem *> listaItem;
    listaItem << new QStandardItem("firefox");
    listaItem << new QStandardItem("Parado");
    listaItem << new QStandardItem("1234");
    listaItem << new QStandardItem("20");
    listaItem << new QStandardItem("Ivanovitch");
    listaItem << new QStandardItem("5");
    listaItem << new QStandardItem("2345");

    model->appendRow(listaItem);
    listaItem.clear();

    listaItem << new QStandardItem("word");
    listaItem << new QStandardItem("Executando");
    listaItem << new QStandardItem("345");
    listaItem << new QStandardItem("1");
    listaItem << new QStandardItem("IMD");
    listaItem << new QStandardItem("6");
    listaItem << new QStandardItem("12345");

    model->appendRow(listaItem);

    ui->tableView->setModel(model);
    ui->tableView->setShowGrid(false);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(4, Qt::AscendingOrder);

 }

MainWindow::~MainWindow()
{
    delete ui;
}
