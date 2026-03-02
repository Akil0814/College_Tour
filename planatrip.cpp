#include "planatrip.h"
#include "ui_planatrip.h"
#include "data_manager.h"
#include <QMessageBox>
#include <QListWidget>
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

// Logic to remove a college when double-clicked
void PlanATrip::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
    if (!item) return;

    QString name = item->text();
    auto idOpt = DataManager::instance()->get_college_id(name);

    if (idOpt.has_value()) {
        // Remove the ID from our background calculation list
        tripStops.removeAll(idOpt.value());

        // Remove the name from the visual UI list
        delete item;

        qDebug() << "Removed:" << name << "| Remaining stops:" << tripStops.size();
    }
}
