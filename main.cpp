#include <QApplication>

#include "home_page.h"
#include "data_manager.h"

#include"data_types.h"//using for data_base_test()

//-------------------testing Database--------------------------//
void data_base_test()
{
    //Database usage reference

    auto data_manager = DataManager::instance();

    /*QVector<college> college_list = data_manager->get_all_colleges();
    for (auto& iter : college_list)
        std::cout << iter.college_id << " " << iter.name.toStdString() << std::endl;*/

    //std::cout << data_manager->last_error().toStdString() << std::endl;
     
    
    //data_manager->is_open();//will give warning

    auto id_ = data_manager->get_college_id("Saddleback College");
    if (id_)
    {
        int id = id_.value();
        std::cout << id << std::endl;
    }
    else
        std::cout << "cant find name" << std::endl;

    auto name_ = data_manager->get_college_name(4);
    if (name_)
    {
        QString name = name_.value();
        std::cout << name.toStdString() << std::endl;
    }
    else
        std::cout << "cant find id" << std::endl;


   /* QVector<souvenir> souvenir_list = data_manager->get_all_souvenirs();
    std::cout << data_manager->last_error().toStdString() << std::endl;
    for (auto& iter : souvenir_list)
        std::cout<<"from college:"<<iter.owner_college_id<<" "<< iter.souvenir_id<<" "<< iter.name.toStdString()<<iter.price<< std::endl;*/

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
        throw std::runtime_error("DataManager init failed");

    //-------------------test Data Manager--------------------------//
    data_base_test();
    //-------------------test Data Manager--------------------------//

    return a.exec();
}
