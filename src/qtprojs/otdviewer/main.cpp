#include "otdviewermain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OtdViewerMain w;
    a.setWindowIcon(w.windowIcon());
    w.show();

    return a.exec();
}
