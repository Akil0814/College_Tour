#include "home_page.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HomePage window;
    window.show();
    a.exec();

    return 0;
}
