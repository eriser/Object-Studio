#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    //setlocale(LC_ALL,"");
    CObjectStudioApplication a(argc, argv);

    a.w.show();

    return a.exec();
}
