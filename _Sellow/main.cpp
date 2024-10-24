#include "hub.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QIcon icon("./icon.ico");
    a.setWindowIcon(icon);

    Hub w;
    w.show();
    return a.exec();
}
