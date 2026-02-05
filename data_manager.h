#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include<iostream>
#include <algorithm>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>

#include "manager.h"

class DataManager :public Manager<DataManager>
{
    friend class Manager<DataManager>;
protected:
    DataManager();
    ~DataManager();
//-------------only for testing-------------------//
public:
    void test_manager()
    {
        std::cout << "DataManager working" << std::endl;
    }
//-------------only for testing-------------------//
public:
    bool init();



private:
    bool open_db();
    bool init_pragmas();
    bool init_schema();
    bool seed_if_empty();


private:
    bool m_init_ed = false;

    QString m_conn_name = "main";
    QString m_db_path;
    QString m_last_error;
};

#endif // DATA_MANAGER_H
