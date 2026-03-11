#include "planatrip.h"
#include "ui_planatrip.h"
#include "data_manager.h"
#include "trip_overview.h"
#include "cart_page.h"

#include <QMessageBox>
#include <QListWidget>
#include <QVector>
#include <QGraphicsOpacityEffect>
#include <QInputDialog>

QVector<int> route_optimize(int start_id, QVector<int> destinations);

PlanATrip::PlanATrip(ShoppingCart* cart, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlanATrip)
    , m_cart(cart)
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
    QVector<college> colleges = DataManager::instance()->get_all_colleges_have_distances();

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

    if (this->parentWidget()) {
        this->parentWidget()->hide();
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

    try {
        // Pass the non-negotiable start and the stops to the fixed function
        QVector<int> optimizedPath = route_optimize(startingCollegeId, tripStops);

        DataManager::instance()->set_current_trip(optimizedPath);
        DataManager::instance()->set_current_trip_index(0);

        // (We can actually delete the QMessageBox here since we are opening the new page immediately!)

        trip_overview *overview = new trip_overview(m_cart);
        overview->show();
        this->close(); // Closes the planner

    } catch (const std::exception &e) {
        // If the algorithm throws an error, catch it here and show a popup instead of crashing!
        QMessageBox::critical(this, "Routing Error", "Could not find a valid path! Ensure all selected campuses are connected.");

        // Optional: Print the exact error to the console for your own debugging
        qDebug() << "Algorithm failed:" << e.what();
    }

}

// --- GUARDRAIL 2: Cannot change start to a college already in the stops list ---
void PlanATrip::on_startingPointDropDown_activated(int index) {
    if (index <= 0) return;

    QString selectedName = ui->startingPointDropDown->currentText();
    auto idOpt = DataManager::instance()->get_college_id(selectedName);

    if (idOpt.has_value()) {
        int id = idOpt.value();

        // If the newly selected start is already in the destination list
        if (tripStops.contains(id)) {
            QMessageBox::warning(this, "Conflict", selectedName + " is already in your Destinations list.\n\nPlease double-click it in the list to remove it before setting it as your start.");
            ui->startingPointDropDown->setCurrentIndex(0); // Reset back to default
        }
    }
}

void PlanATrip::on_tripStopsDropDown_activated(int index) {
    if (index <= 0) return;

    QString selectedName = ui->tripStopsDropDown->currentText();

    // --- GUARDRAIL 1: Cannot add the starting college to stops ---
    if (selectedName == ui->startingPointDropDown->currentText()) {
        QMessageBox::warning(this, "Invalid Stop", "You cannot add your Starting College as a destination stop.");
        ui->tripStopsDropDown->setCurrentIndex(0); // Reset dropdown
        return;
    }

    auto idOpt = DataManager::instance()->get_college_id(selectedName);

    if (idOpt.has_value()) {
        int id = idOpt.value();

        if (!tripStops.contains(id)) {
            tripStops.append(id);
            ui->listWidget->addItem(selectedName);
            ui->tripStopsDropDown->setCurrentIndex(0); // Reset dropdown after successful add
        } else {
            QMessageBox::information(this, "Already Added", selectedName + " is already in your trip.");
            ui->tripStopsDropDown->setCurrentIndex(0);
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

//added

void PlanATrip::on_premadeTripsDropDown_activated(int index) {
    if (index <= 0) return; // Ignore the placeholder

    QString startCollege;
    QStringList stops;

    if (index == 1) {
        // --- Pre-made Trip 1 ---
        startCollege = "Saddleback College";
        stops << "Arizona State University" << "Massachusetts Institute of Technology (MIT)"
              << "Northwestern" << "Ohio State University" << "University of  Michigan"
              << "University of California, Irvine (UCI)" << "University of California, Los Angeles (UCLA)"
              << "University of Oregon" << "University of the Pacific" << "University of Wisconsin";

    } else if (index == 2) {
        // --- Pre-made Trip 2 ---
        startCollege = "University of California, Irvine (UCI)";
        stops << "Massachusetts Institute of Technology (MIT)" << "Northwestern"
              << "Ohio State University" << "University of  Michigan"
              << "University of California, Los Angeles (UCLA)" << "University of Oregon"
              << "University of the Pacific" << "University of Wisconsin"
              << "Arizona State University" << "Saddleback College"
              << "California State University, Fullerton" << "University of Texas";

    } else if (index == 3) {
        // --- NEW: ASU Custom Trip (Dynamic) ---
        bool ok;
        int n = QInputDialog::getInt(this, "ASU Custom Trip",
                                     "Enter total number of colleges to visit (1-11):",
                                     11, 1, 11, 1, &ok);

        // If the user hit Cancel, silently reset the dropdown and abort
        if (!ok) {
            ui->premadeTripsDropDown->setCurrentIndex(0);
            return;
        }

        startCollege = "Arizona State University";

        QStringList originalColleges = {
            "Massachusetts Institute of Technology (MIT)", "Northwestern", "Ohio State University",
            "Saddleback College", "University of  Michigan", "University of California, Irvine (UCI)",
            "University of California, Los Angeles (UCLA)", "University of Oregon",
            "University of the Pacific", "University of Wisconsin"
        };

        int asuId = DataManager::instance()->get_college_id(startCollege).value_or(-1);
        if (asuId == -1) return;

        // Fetch IDs for the algorithm
        QVector<int> destinations;
        for (const QString& name : originalColleges) {
            auto idOpt = DataManager::instance()->get_college_id(name);
            if (idOpt.has_value()) destinations.append(idOpt.value());
        }

        // Run the Greedy Algorithm
        extern QVector<int> route_optimize(int start_id, QVector<int> destinations);
        QVector<int> full_optimized_trip = route_optimize(asuId, destinations);

        // Truncate the list to the user's number.
        // Note: We start loop at i=1 because i=0 is ASU (which goes in the Start Dropdown!)
        for (int i = 1; i < n && i < full_optimized_trip.size(); ++i) {
            int collegeId = full_optimized_trip[i];
            QString collegeName = DataManager::instance()->get_college_name(collegeId).value_or("");
            if (!collegeName.isEmpty()) {
                stops << collegeName; // Add dynamically ordered colleges to the stops list
            }
        }
    }

    // ==========================================
    // UI POPULATION LOGIC (Runs for all 3 trips)
    // ==========================================

    // 1. Clear existing selections
    tripStops.clear();
    ui->listWidget->clear();

    // 2. Set the Starting College Dropdown
    int startIndex = ui->startingPointDropDown->findText(startCollege, Qt::MatchExactly);
    if (startIndex != -1) {
        ui->startingPointDropDown->setCurrentIndex(startIndex);

        auto startIdOpt = DataManager::instance()->get_college_id(startCollege);
        if (startIdOpt.has_value()) {
            startingCollegeId = startIdOpt.value();
        }
    }

    // 3. Add the destinations to the list widget and backend vector
    for (const QString& stopName : stops) {
        auto idOpt = DataManager::instance()->get_college_id(stopName);

        if (idOpt.has_value()) {
            tripStops.append(idOpt.value());
            ui->listWidget->addItem(stopName);
        }
    }
}
