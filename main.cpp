#include "adsbsorter.h"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AdsbSorter w;
    w.show();

    return a.exec();
}
