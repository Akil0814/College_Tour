#include "planatrip.h"
#include "ui_planatrip.h"

PlanATrip::PlanATrip(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlanATrip)
{
    ui->setupUi(this);
}

PlanATrip::~PlanATrip()
{
    delete ui;
}
