#include <QApplication>

#include "home_page.h"
#include "data_manager.h"

#include"data_types.h"//using for data_base_test()

//-------------------testing Database--------------------------//
void data_base_test()
{
    // Demo for DataManager API usage

    // Get the singleton instance
    auto data_manager = DataManager::instance();

    // Check whether the database connection is currently open.
    // NOTE: is_open(), last_error() is marked [[nodiscard]], so calling it without using the return value
    //       will trigger a compiler warning (by design).
    // Example:
    // data_manager->is_open();//will warn
    if (!data_manager->is_open())
    {
        // Fail fast: stop the test early if the database is not available.
        // last_error() should contain reason (open failed, missing file, bad path, etc.).
        std::cerr << "database is not open: "
            << data_manager->last_error().toStdString()
            << std::endl;
        return;
    }

    // ----------- Query: fetch all colleges -----------
    QVector<college> college_list = data_manager->get_all_colleges();

    // Print each college record (id + name).
    for (auto& iter : college_list)
        std::cout << iter.college_id << " " << iter.name.toStdString() << std::endl;

    // Print the last error message (empty if no error).
    std::cout << data_manager->last_error().toStdString() << std::endl;

    // ----------- Query: get college id by college name -----------
    auto id_ = data_manager->get_college_id("Saddleback College");
    if (id_)
    {
        // std::optional holds a value: extract and print it.
        int id = id_.value();
        std::cout << id << std::endl;
    }
    else
    {
        // std::optional is empty: name not found.
        std::cout << "cant find name" << std::endl;
    }

    // ----------- Query: get college name by college id -----------
    auto name_ = data_manager->get_college_name(4);
    if (name_)
    {
        // Extract and print the college name.
        QString name = name_.value();
        std::cout << name.toStdString() << std::endl;
    }
    else
    {
        // Invalid id or missing record.
        std::cout << "cant find id" << std::endl;
    }

    // ----------- Query: fetch all souvenirs -----------
    QVector<souvenir> souvenir_list = data_manager->get_all_souvenirs();

    // Print each souvenir record (owner college id, souvenir id, name, price).
    for (auto& iter : souvenir_list)
        std::cout << "from college:" << iter.owner_college_id << " "
        << iter.souvenir_id << " "
        << iter.name.toStdString()
        << iter.price << std::endl;

    data_manager->test_manager();
}
//-------------------testing Database--------------------------//

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HomePage window;
    window.show();

    //init database
    if (!DataManager::instance()->init())
    {
        qDebug() << "DataManager init failed:" << DataManager::instance()->last_error();
        throw std::runtime_error("DataManager init failed");
    }

    //-------------------test Data Manager--------------------------//
    data_base_test();//used for quick debugging/smoke testing; comment out once everything works.
    //-------------------test Data Manager--------------------------//

    return a.exec();
}
