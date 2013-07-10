#include "dialeqt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialeqt w;
    w.show();
    
    return a.exec();
}
