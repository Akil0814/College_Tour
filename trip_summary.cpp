#include "trip_summary.h"
#include "ui_trip_summary.h"

TripSummary::TripSummary(CartPage *cart_page, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TripSummary)
{
    ui->setupUi(this);

    // Save pointer to cart page for clear / refresh cart
    this->cart_page = cart_page;

    // Init table headers
    QStringList tableHeaders;
    tableHeaders << "University Visited" << "Items Purchased" << "Spent at Location";
    int numTableHeaders = tableHeaders.size();

    ui->cartTable->setColumnCount(numTableHeaders);
    ui->cartTable->setHorizontalHeaderLabels(tableHeaders);
    ui->cartTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

TripSummary::~TripSummary()
{
    delete ui;
}

void TripSummary::populateTable(ShoppingCart *cart, DistanceTracker *dt)
{
    // Get locations visited
    QVector<int> trip_ids = DataManager::instance()->get_current_trip();

    // Get items and cost for each location
    for (auto id : trip_ids)
    {
        std::optional<QString>location = DataManager::instance()->get_college_name(id);
        int locationNumPurchased = cart->total_items_for_college(id);
        double locationCost = cart->total_cost_for_college(id);

        // Currently only used to validate number purchased (show all items purchased)
        QVector<ShoppingCart::Item> locationItems = cart->items_for_college(id);

        // Output error message
        if (!location.has_value())
        {
            qDebug() << "|Summary| School name not retrieved from id";
        }
        if (locationNumPurchased != locationItems.size())
        {
            qDebug() << "|Summary| Number purchased and number of items do not match";
        }

        // Create new row at end of table
        int row = ui->cartTable->rowCount();
        ui->cartTable->insertRow(row);

        // Add row to table
        ui->cartTable->setItem(row, 0, new QTableWidgetItem(location.value()));
        ui->cartTable->setItem(row, 1, new QTableWidgetItem(QString::number(locationNumPurchased)));
        ui->cartTable->setItem(row, 2, new QTableWidgetItem(QString::number(locationCost, 'f', 2)));
    }

    // Resize table
    ui->cartTable->resizeColumnsToContents();

    // Populate stats below table
    populateStats(trip_ids, cart, dt);
}

void TripSummary::populateStats(QVector<int>& locations, ShoppingCart *cart, DistanceTracker *dt)
{
    // Retrieve, num locations, num purchased, total spent, total distance
    int numLocations = locations.size();
    double totalSpent = cart->grand_total();
    int totalDistance = dt->get_total_distance();
    int numPurchased = 0;

    for (ShoppingCart::Item i : cart->all_items())
    {
        numPurchased += i.quantity;
    }

    // Update Labels
    ui->locationsVisitedLabel->setText("Number of Locations Visited: " + QString::number(numLocations));
    ui->itemsPurchasedLabel->setText("Number of Items Purchased: " + QString::number(numPurchased));
    ui->totalSpentLabel->setText("Total Spent: " + QString::number(totalSpent, 'f', 2));
    ui->totalDistanceTravelledLabel->setText("Total Distance Travelled: " + QString::number(totalDistance));
}

void TripSummary::on_exitSummaryButton_clicked()
{
    // Clear cart - function crashes program
    // cart_page->clearCartAndRefresh();

    // Search through all running windows in the application
    for (QWidget *widget : QApplication::topLevelWidgets()) {

        // If the window is the HomePage, unhide it
        if (HomePage *home = qobject_cast<HomePage*>(widget)) {
            home->show();
            break;
        }
    }

    // Close the summary window
    this->close();
}
