#include "home_page.h"
#include "data_sqLite.cpp"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HomePage window;
    window.show();

    //test sqlite
    if (!initSqlite("main")) return 1;

    // demo
    {
        QSqlDatabase db = QSqlDatabase::database("main");
        QSqlQuery q(db);
        q.prepare("INSERT INTO t(name) VALUES(?);");
        q.addBindValue("hello");
        if (!q.exec()) {
            qDebug() << "INSERT failed:" << q.lastError().text();
            return 1;
        }
    }

    //test sqlite

    qDebug() << "ok";
    a.exec();


    return 0;
}
