#include "planatrip.h"
#include "ui_planatrip.h"
#include "data_manager.h"
#include <QMessageBox>
#include <QVector>

QVector<int> route_optimize(int start_id, QVector<int> destinations);

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
    ui->startingPointDropDown->clear();
    ui->tripStopsDropDown->clear();

    // 3. Added a default prompt
    ui->startingPointDropDown->addItem("--- Select Starting College ---");

    // 4. Iterate through the database results and add names to dropdowns
    for (const auto& col : colleges) {
        ui->startingPointDropDown->addItem(col.name);
        ui->tripStopsDropDown->addItem(col.name);
    }
}

void PlanATrip::on_addStopButton_clicked() {
    // 1. Get the current selection from the dropdown
    QString selectedName = ui->tripStopsDropDown->currentText();

    // 2. Get the ID from DataManager
    auto idOpt = DataManager::instance()->get_college_id(selectedName);

    if (idOpt.has_value()) {
        int id = idOpt.value();

        // 3. Prevent duplicates in the trip
        if (!tripStops.contains(id)) {
            tripStops.append(id);
            ui->listWidget->addItem(selectedName); // Show it to the user
        }
    }
}

void PlanATrip::on_goButton_clicked() {
    if (ui->startingPointDropDown->currentIndex() <= 0) {
        QMessageBox::warning(this, "Selection Required", "Please select a starting college.");
        return;
    }

    if (tripStops.isEmpty()) {
        QMessageBox::warning(this, "Empty Trip", "Please add at least one stop to your trip.");
        return;
    }

    // Pass the non-negotiable start and the stops to the fixed function
    QVector<int> optimizedPath = route_optimize(startingCollegeId, tripStops);

    QMessageBox::information(this, "Route Optimized",
                             "Shortest path found for " + QString::number(optimizedPath.size()) + " colleges!");
}

void PlanATrip::on_tripStopsDropDown_activated(int index) {
    if (index <= 0) return;

    QString selectedName = ui->tripStopsDropDown->currentText();
    auto idOpt = DataManager::instance()->get_college_id(selectedName);

    if (idOpt.has_value()) {
        int id = idOpt.value();

        if (!tripStops.contains(id)) {
            tripStops.append(id);
            ui->listWidget->addItem(selectedName);
        } else {
            QMessageBox::information(this, "Already Added", selectedName + " is already in your trip.");
        }
    }
}
