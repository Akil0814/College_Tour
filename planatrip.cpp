#include "planatrip.h"
#include "ui_planatrip.h"
#include "data_manager.h"
#include "trip_overview.h"
#include <QMessageBox>
#include <QListWidget>
#include <QVector>
#include <QGraphicsOpacityEffect>

QVector<int> route_optimize(int start_id, QVector<int> destinations);

PlanATrip::PlanATrip(ShoppingCart* m_cart, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlanATrip)
{
    bg = new QLabel(this);
    bg->setPixmap(QPixmap(":/res/res/saddleback-college-gateway-building-1050x750-compact.png"));
    bg->setScaledContents(true);
    bg->resize(size());
    bg->setStyleSheet("background: transparent;");
    bg->setGraphicsEffect(new QGraphicsOpacityEffect(bg));
    auto *effect = new QGraphicsOpacityEffect(bg);
    effect->setOpacity(0.4);
    bg->setGraphicsEffect(effect);
    ui->setupUi(this);

    // Fill the dropdowns as soon as the window is created
    populateColleges();
}

PlanATrip::~PlanATrip()
{
    delete ui;
}

void PlanATrip::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    bg->resize(size());
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
    ui->tripStopsDropDown->addItem("--- Select a Stop to Add ---");

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

    QString startName = ui->startingPointDropDown->currentText();
    auto idOpt = DataManager::instance()->get_college_id(startName);

    if (!idOpt.has_value()) {
        QMessageBox::critical(this, "Error", "Could not find starting college in database.");
        return;
    }

    // Assign the value to the variable your optimizer uses
    startingCollegeId = idOpt.value();

    // Remove the starting college from the destinations so it doesn't try to visit itself!
    tripStops.removeAll(startingCollegeId);

    // Just in case the user ONLY added the starting college to the stops list:
    if (tripStops.isEmpty()) {
        QMessageBox::warning(this, "Not Enough Stops", "Please add at least one different destination to your trip!");
        return;
    }

    // --- ADD THE TRY/CATCH BLOCK HERE ---
    try {
        // Pass the non-negotiable start and the stops to the fixed function
        QVector<int> optimizedPath = route_optimize(startingCollegeId, tripStops);

        DataManager::instance()->set_current_trip(optimizedPath);
        DataManager::instance()->set_current_trip_index(0);

        // (We can actually delete the QMessageBox here since we are opening the new page immediately!)

        trip_overview *overview = new trip_overview();
        overview->show();
        this->close(); // Closes the planner

    } catch (const std::exception &e) {
        // If the algorithm throws an error, catch it here and show a popup instead of crashing!
        QMessageBox::critical(this, "Routing Error", "Could not find a valid path! Ensure all selected campuses are connected.");

        // Optional: Print the exact error to the console for your own debugging
        qDebug() << "Algorithm failed:" << e.what();
    }

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
