#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

  model = new QStandardItemModel();
  QStringList headers;
  headers << tr("Nome") << tr("Status") << tr("PID") << tr("PPID") << tr("Usuário") << tr("Threads") << tr("Trocas de Contexto");
  model->setHorizontalHeaderLabels(headers);


// model->setHorizontalHeaderItem( 0, new QStandardItem( "Nome" ) );
// model->setHorizontalHeaderItem( 1, new QStandardItem( "Status" ) );
 // model->setHorizontalHeaderItem( 2, new QStandardItem( "PID" ) );


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
