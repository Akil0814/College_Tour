#include "cart_page.h"
#include "distance_tracker.h"
#include "ui_cart_page.h"
#include "data_manager.h"
#include "trip_summary.h"
#include "distance_tracker.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <optional>
#include <QIntValidator>

CartPage::CartPage(ShoppingCart& cart, DataManager* dm, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::CartPage),
    m_cart(cart),
    m_dm(dm)
{
    ui->setupUi(this);

    //setting font of output in tables
    QFont tableFont("Segoe UI", 17);
    ui->tableSouvenirs->setFont(tableFont);
    ui->tableCart->setFont(tableFont);

    setWindowTitle("Campus Page");

    //making the columns evenly distrubted
    ui->tableSouvenirs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableSouvenirs->verticalHeader()->setVisible(false);
    ui->tableSouvenirs->setAlternatingRowColors(true);

    // left table: souvenirs for this campus
    ui->tableSouvenirs->setColumnCount(2);
    ui->tableSouvenirs->setHorizontalHeaderLabels(QStringList() << "Souvenir" << "Cost");

    ui->tableSouvenirs->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableSouvenirs->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableSouvenirs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSouvenirs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableSouvenirs->verticalHeader()->setVisible(false);

    // right table: items already bought at this campus
    ui->tableCart->setColumnCount(4);
    ui->tableCart->setHorizontalHeaderLabels(
        QStringList() << "Souvenir" << "Qty" << "Unit Price" << "Subtotal"
        );
    ui->tableCart->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCart->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableCart->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCart->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCart->verticalHeader()->setVisible(false);

    //for validating input into removal entry
    QIntValidator *removeValidator = new QIntValidator(1, 999, this);
    ui->qtyRemoveInput->setValidator(removeValidator);

    //for validating input into addding entry
    QIntValidator *qtyValidator = new QIntValidator(1, 999, this);
    ui->qtyInput->setValidator(qtyValidator);

    ui->qtyInput->setText("1");
    ui->qtyInput->setMaxLength(3);   // optional: prevents huge values

    ui->labelCollege->setText("Campus Page");
    ui->labelCampusTotal->setText("Campus Total: $0.00");
    ui->labelGrandTotal->setText("Grand Total: $0.00");
}

CartPage::~CartPage()
{
    delete ui;
}

//resetting for every college
void CartPage::openForCollege(int college_id)
{
    m_college_id = college_id;

    refreshHeader();
    refreshSouvenirs();
    refreshCart();
}

//refreshing the campus header at top
void CartPage::refreshHeader()
{
    if (!m_dm || m_college_id < 0)
    {
        ui->labelCollege->setText("Unknown Campus");
        return;
    }

    std::optional<QString> nameOpt = m_dm->get_college_name(m_college_id);

    if (nameOpt.has_value())
        ui->labelCollege->setText("College: " + nameOpt.value());
    else
        ui->labelCollege->setText("College: Unknown Campus");

    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int currentIndex = DataManager::instance()->get_current_trip_index();

   // Dynamically change the button text based on where we are in the trip
    if (currentIndex >= fullTrip.size() - 1)
        ui->btnClose->setText("Finish Trip");
    else if ((currentIndex + 1) % 5 == 0)
        ui->btnClose->setText("Next Leg Overview");
    else
        ui->btnClose->setText("Next College");
}

//refreshing the souvenir options
void CartPage::refreshSouvenirs()
{
    ui->tableSouvenirs->setRowCount(0);

    if (!m_dm || m_college_id < 0)
        return;

    QVector<souvenir> souvenirs = m_dm->get_all_souvenirs_from_college(m_college_id);

    for (int row = 0; row < souvenirs.size(); ++row)
    {
        const souvenir& s = souvenirs[row];
        ui->tableSouvenirs->insertRow(row);

        QTableWidgetItem* nameItem = new QTableWidgetItem(s.name);
        nameItem->setData(Qt::UserRole, s.souvenir_id);   // hidden ID

        ui->tableSouvenirs->setItem(row, 0, nameItem);
        ui->tableSouvenirs->setItem(row, 1, new QTableWidgetItem("$" + QString::number(s.price, 'f', 2)));
    }
}

//refreshing cart at every college
void CartPage::refreshCart()
{
    ui->tableCart->setRowCount(0);

    if (m_college_id < 0)
        return;

    QVector<ShoppingCart::Item> items = m_cart.items_for_college(m_college_id);

    for (int row = 0; row < items.size(); ++row)
    {
        const ShoppingCart::Item& item = items[row];
        double subtotal = item.unit_price * item.quantity;

        ui->tableCart->insertRow(row);

        QTableWidgetItem *nameItem = new QTableWidgetItem(item.name);
        nameItem->setData(Qt::UserRole, item.souvenir_id);   // store souvenir ID
        ui->tableCart->setItem(row, 0, nameItem);

        ui->tableCart->setItem(row, 1, new QTableWidgetItem(QString::number(item.quantity)));
        ui->tableCart->setItem(row, 2, new QTableWidgetItem("$" + QString::number(item.unit_price, 'f', 2)));
        ui->tableCart->setItem(row, 3, new QTableWidgetItem("$" + QString::number(subtotal, 'f', 2)));
    }

    ui->labelCampusTotal->setText(
        "Campus Total: $" + QString::number(m_cart.total_cost_for_college(m_college_id), 'f', 2)
        );

    ui->labelGrandTotal->setText(
        "Grand Total: $" + QString::number(m_cart.grand_total(), 'f', 2)
        );
}

//to add to cart
void CartPage::on_btnAdd_clicked()
{
    if (!m_dm || m_college_id < 0)
    {
        QMessageBox::warning(this, "Error", "No campus is loaded.");
        return;
    }

    QModelIndexList selectedRows = ui->tableSouvenirs->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, "Add To Cart", "Please select a souvenir first.");
        return;
    }

    int row = selectedRows.first().row();
    QTableWidgetItem* nameItem = ui->tableSouvenirs->item(row, 0);
    int souvenir_id = nameItem->data(Qt::UserRole).toInt();

    if (!nameItem)
    {
        QMessageBox::warning(this, "Add To Cart", "Invalid souvenir selection.");
        return;
    }

    //adding the label text instead of spinbox
    QString qtyText = ui->qtyInput->text().trimmed();

    if (qtyText.isEmpty())
    {
        QMessageBox::warning(this, "Add To Cart", "Please enter a quantity.");
        return;
    }

    bool converted = false;
    int qty = qtyText.toInt(&converted);

    if (!converted || qty <= 0 || qty > 999)
    {
        QMessageBox::warning(this, "Add To Cart", "Please enter a valid quantity from 1 to 999.");
        ui->qtyInput->setText("1");
        return;
    }

    bool ok = m_cart.add_item(m_dm, souvenir_id, qty);

    if (!ok)
    {
        QMessageBox::warning(this, "Add To Cart", "Could not add souvenir to cart.");
        return;
    }

    refreshCart();

    ui->qtyInput->setText("1");
}

//to remove from cart
void CartPage::on_btnRemove_clicked()
{

    int row = ui->tableCart->currentRow();

    qDebug() << "Row:" << row;

    if (row < 0) {
        QMessageBox::warning(this, "Remove", "Please select an item first.");
        return;
    }

    int souvenir_id = ui->tableCart->item(row, 0)->data(Qt::UserRole).toInt();

    QString text = ui->qtyRemoveInput->text().trimmed();
    if (text.isEmpty()) return;

    int removeQty = text.toInt();
    if (removeQty <= 0) return;

    int currentQty = m_cart.quantity(souvenir_id);

    if (removeQty >= currentQty)
        m_cart.remove(souvenir_id);          // remove all
    else
        m_cart.set_quantity(souvenir_id, currentQty - removeQty); // subtract

    refreshCart();
    ui->qtyRemoveInput->setText("1");
}

//to move onto the next college
void CartPage::on_btnClose_clicked()
{
    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int currentIndex = DataManager::instance()->get_current_trip_index();

    // 1. Advance the index
    currentIndex++;
    DataManager::instance()->set_current_trip_index(currentIndex);

    // 2. Are we at the very end of the trip?
    if (currentIndex >= fullTrip.size())
    {
        // Compile the Distance Tracker
        DistanceTracker dt(DataManager::instance());
        for (int id : fullTrip) {
            std::optional<QString> name = DataManager::instance()->get_college_name(id);
            if (name.has_value()) {
                dt.location_changed(name.value());
            }
        }

        // Open the Summary Page directly from the last cart!
        TripSummary *summaryPage = new TripSummary(nullptr);
        summaryPage->populateTable(&m_cart, &dt);
        summaryPage->setAttribute(Qt::WA_DeleteOnClose);
        summaryPage->show();

        accept(); // Close the Cart Window
        return;
    }

    // 3. Did we just finish a leg of 5?
    if (currentIndex % 5 == 0)
    {
        accept(); // Close the cart to reveal the Trip Overview in the background!
        return;
    }

    // 4. Otherwise, immediately load the next campus into the cart window
    int nextCollegeId = fullTrip[currentIndex];
    openForCollege(nextCollegeId);
}

//resizing the page
void CartPage::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    if (!ui->contentWidget)
        return;

    int x = (width() - ui->contentWidget->width()) / 2;
    int y = (height() - ui->contentWidget->height()) / 2;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    ui->contentWidget->move(x, y);
}
