#include "overflowpalettewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OverflowPaletteWindow w;

    w.show();
    return a.exec();
}
