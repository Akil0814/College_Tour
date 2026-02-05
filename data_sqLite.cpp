#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

//testing sqlite

static bool initSqlite(const QString& connectionName = "main")
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    const QString dbPath = dir + "/test.sqlite";

    QSqlDatabase db = QSqlDatabase::contains(connectionName)
                          ? QSqlDatabase::database(connectionName)
                          : QSqlDatabase::addDatabase("QSQLITE", connectionName);

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "DB open failed:" << db.lastError().text();
        return false;
    }

    QSqlQuery q(db);

    if (!q.exec("CREATE TABLE IF NOT EXISTS t(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL);")) {
        qDebug() << "CREATE TABLE failed:" << q.lastError().text();
        return false;
    }

    qDebug() << "DB ready at:" << dbPath;
    return true;
}
