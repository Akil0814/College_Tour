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
#include <QFile>
#include <QFileInfo>
#include <QTextStream>


#include "manager.h"
#include "data_types.h"

class DataManager :public Manager<DataManager>
{
    friend class Manager<DataManager>;

protected:
    DataManager()=default;
    ~DataManager() =default;

public:
//-------------only for testing-------------------//
    void test_manager()
    {
        std::cout << "DataManager working" << std::endl;
    }
//-------------only for testing-------------------//

public:
    //initializes database
    [[nodiscard]] bool init();

    //returns whether the database connection is currently open and usable
    [[nodiscard]] bool is_open() const;

    //returns the last error message produced by a failed database operation
    [[nodiscard]] QString last_error() const;

    //-------------------------------related to colleges getter-------------------------------------//
    
    //returns a list of all colleges currently stored in the database
    QVector<college> get_all_colleges() const;

    //returns nullopt if not found
    std::optional<int> get_college_id(const QString& college_name) const;

    //returns nullopt if not found
    std::optional<QString> get_college_name(int college_id) const;

    // returns the direct distances from the given college_id to all other colleges.
    QVector<distance_to> get_distances_from_college(int college_id) const;

    //returns the direct distance between two colleges, returns nullopt if no edge exists
    std::optional<double> get_distance_between_college(int college_id_1, int college_id_2) const;

    //returns the direct distance between two colleges, returns nullopt if no edge exists
    std::optional<double> get_distance_between_college(const QString& college_name_1, const QString& college_name_2) const;



    //-------------------------------related to souvenirs getter-------------------------------------//

    //returns a list of all souvenirs across all colleges
    QVector<souvenir> get_all_souvenirs() const;

    //returns all souvenirs that belong to the specified college_id
    QVector<souvenir> get_all_souvenirs_from_college(int college_id) const;

    //returns all souvenirs that belong to the specified college_name
    QVector<souvenir> get_all_souvenirs_from_college(const QString& college_name) const;

    //searching a souvenir_id by college_id and souvenir name; returns nullopt if not found.
    std::optional<int> get_souvenir_id(int college_id, const QString& souvenir_name) const;

    //returns the souvenir record by souvenir_id; returns nullopt if not found.
    std::optional<souvenir> get_souvenir(int souvenir_id) const;



    //-------------------------------related to college operational-------------------------------------//

    //deletes a college by id; returns true on success
    bool delete_college(int college_id);

    //returns id if successfully added or already exists, nullopt on error
    std::optional<int> add_college(const QString& new_college_name);

    //set the direct distance between two colleges; returns true on success
    bool set_distance_between_college(int college_id_1, int college_id_2, double miles);



    //-------------------------------related to souvenir operational-------------------------------------//

    //return souvenir id if successfully added
    //price check fail//wrong error message when college_id does not exist
    std::optional<int> add_souvenir(const souvenir& s);

    //return souvenir id if successfully added
    //price check fail//wrong error message when college_id does not exist
    std::optional<int> add_souvenir(int college_id, const QString& name, double price);


    bool adjust_souvenir_price(int college_id, const QString& souvenir_name,double price);

    bool adjust_souvenir_price(int souvenir_id, double price);


    //return true if successfully deleted
    bool delete_souvenir(int souvenir_id);

    //-------------------------------read file-------------------------------------//

    bool read_schoole_flie();



private:

    bool open_db();
    bool init_pragmas();
    bool init_schema();
    bool seed_if_empty();

    QSqlDatabase get_db_or_set_error() const;
    bool prepare_and_exec(QSqlQuery& q, const QString& sql) const;
    bool prepare_or_set_error(QSqlQuery& q, const QString& sql) const;
    bool exec_or_set_error(QSqlQuery& q) const;



private:

    bool m_init_ed = false;

    QString m_conn_name = "main";
    QString m_db_path;
    mutable QString m_last_error;
};

#endif // DATA_MANAGER_H
