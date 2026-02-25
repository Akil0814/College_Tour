#include "planatrip.h"
#include "ui_planatrip.h"
#include "data_manager.h"

PlanATrip::PlanATrip(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlanATrip)
{
    ui->setupUi(this);

    // Fill the dropdowns as soon as the window is created
    populateColleges();
}

PlanATrip::~PlanATrip()
{
    delete ui;
}

void PlanATrip::populateColleges()
{
    // 1. Get the list of all colleges from the DataManager
    QVector<college> colleges = DataManager::instance()->get_all_colleges();

    // 2. Clear existing items to avoid duplicates if called multiple times
    // Replace these names with the actual objectNames you set in Designer (e.g., startComboBox)
    ui->startingPointDropDown->clear();
    ui->tripStopsDropDown->clear();

    // 3. Add a default prompt
    ui->startingPointDropDown->addItem("--- Select Starting College ---");

    // 4. Iterate through the database results and add names to dropdowns
    for (const auto& col : colleges) {
        ui->startingPointDropDown->addItem(col.name);
        ui->tripStopsDropDown->addItem(col.name);
    }
}
