#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include<iostream>
#include <algorithm>
#include <functional>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QVector>
#include <optional>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QDir>
#include <QSet>
#include <QHash>

#include "manager.h"
#include "data_types.h"

class DataManager :public Manager<DataManager>
{
    friend class Manager<DataManager>;

protected:
    DataManager();
    ~DataManager();

public:
//-------------only for testing-------------------//
    void test_manager()
    {
        std::cout << "DataManager working" << std::endl;
    }
//-------------only for testing-------------------//

public:

    bool init();
    bool is_open() const;
    QString last_error() const;


    // std::optional<typename> will return std::nullopt if an error occurred.

    QVector<college> get_all_colleges() const;
    std::optional<int> get_college_id(const QString& college_name) const;
    std::optional<QString> get_college_name(int college_id) const;
    QVector<neighbor_distance> get_distances_from(int from_id) const;
    std::optional<double> get_distance_by_college_id(int college_id_1, int college_id_2) const;
    //std::optional<double> get_distance_by_college_name(const QString& from_name, const QString& to_name) const;


    QVector<souvenir> get_all_souvenir_by_college_id(int college_id) const;
    std::optional<int> get_souvenir_id(int college_id, const QString& souvenir_name) const;
    std::optional<souvenir> get_souvenir(int souvenir_id) const;


    QVector<souvenir> get_souvenirs_by_college_name(const QString& college_name) const;
    QVector<souvenir> get_souvenirs_by_college_id(int college_id) const;

    bool add_college(const QString& name);
    bool delete_college(int college_id);
    bool set_distance_between_college(int college_id_1, int college_id_2, double miles);

    bool add_souvenir(const souvenir& s);
    bool add_souvenir(int college_id, const QString& name, double price);
    bool delete_souvenir(int souvenir_id);


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
