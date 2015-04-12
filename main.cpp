#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //Procurar na pasta proc pastas que o nome são números: são os processos

    return a.exec();
}
