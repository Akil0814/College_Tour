#include <QDebug>
#include <QApplication>

#include "home_page.h"
#include "data_manager.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HomePage window;
    window.show();

    //-------------------test Data Manager--------------------------//

    DataManager::instance()->test_manager();

    if (!DataManager::instance()->init())
        throw std::runtime_error("DataManager init failed");

    auto data_manager = DataManager::instance();
    data_manager->test_manager();

    //-------------------test Data Manager--------------------------//

    qDebug() << "ok";


    return a.exec();
}
