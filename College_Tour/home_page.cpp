#include "home_page.h"
#include "./ui_homepage.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    this->setWindowIcon(QIcon(":/res/Saddleback.png"));
    ui->setupUi(this);
}

HomePage::~HomePage()
{
    delete ui;
}
