#include "initauth.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitAuth w;
    w.show();
    return a.exec();
}
