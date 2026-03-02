#include "cart_page.h"
#include "ui_cart_page.h"
#include "data_manager.h"
#include <QMessageBox>
#include <QTableWidgetItem>

CartPage::CartPage(ShoppingCart& cart, DataManager* dm, QWidget* parent)
    : QDialog(parent), ui(new Ui::CartPage), m_cart(cart), m_dm(dm)
{
    ui->setupUi(this);
    ui->tableSouvenirs->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableSouvenirs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCart->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCart->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

CartPage::~CartPage(){ delete ui; }

void CartPage::openForCollege(int college_id)
{
    m_college_id = college_id;
    ui->labelCollege->setText("College ID: " + QString::number(college_id));
    refreshSouvenirs();
    refreshCart();
}

void CartPage::refreshSouvenirs()
{
    ui->tableSouvenirs->setRowCount(0);
    auto souvenirs = m_dm->get_all_souvenirs_from_college(m_college_id);
    ui->tableSouvenirs->setRowCount(souvenirs.size());

    for (int i = 0; i < souvenirs.size(); ++i)
    {
        const auto& s = souvenirs[i];
        ui->tableSouvenirs->setItem(i, 0, new QTableWidgetItem(QString::number(s.souvenir_id)));
        ui->tableSouvenirs->setItem(i, 1, new QTableWidgetItem(s.name));
        ui->tableSouvenirs->setItem(i, 2, new QTableWidgetItem("$" + QString::number(s.price, 'f', 2)));
    }
}

void CartPage::refreshCart()
{
    ui->tableCart->setRowCount(0);
    auto items = m_cart.items_for_college(m_college_id);
    ui->tableCart->setRowCount(items.size());

    for (int i = 0; i < items.size(); ++i)
    {
        const auto& it = items[i];
        double subtotal = it.unit_price * it.quantity;
        ui->tableCart->setItem(i, 0, new QTableWidgetItem(it.name));
        ui->tableCart->setItem(i, 1, new QTableWidgetItem(QString::number(it.quantity)));
        ui->tableCart->setItem(i, 2, new QTableWidgetItem("$" + QString::number(it.unit_price, 'f', 2)));
        ui->tableCart->setItem(i, 3, new QTableWidgetItem("$" + QString::number(subtotal, 'f', 2)));
    }

    ui->labelCampusTotal->setText("Campus total: $" + QString::number(m_cart.total_cost_for_college(m_college_id), 'f', 2));
    ui->labelGrandTotal->setText("Grand total: $" + QString::number(m_cart.grand_total(), 'f', 2));
}

void CartPage::on_btnAdd_clicked()
{
    auto sel = ui->tableSouvenirs->selectionModel()->selectedRows();
    if (sel.isEmpty()) { QMessageBox::warning(this,"Add","Select a souvenir."); return; }

    int row = sel.first().row();
    int souvenir_id = ui->tableSouvenirs->item(row,0)->text().toInt();
    int qty = ui->spinQty->value();

    if (!m_cart.add_item(m_dm, souvenir_id, qty))
        QMessageBox::warning(this,"Add","Could not add item.");

    refreshCart();
}

void CartPage::on_btnRemove_clicked()
{
    auto sel = ui->tableCart->selectionModel()->selectedRows();
    if (sel.isEmpty())
    {
        QMessageBox::warning(this, "Remove", "Select a cart item.");
        return;
    }

    int row = sel.first().row();
    auto items = m_cart.items_for_college(m_college_id);
    if (row < 0 || row >= items.size()) return;

    m_cart.remove(items[row].souvenir_id);
    refreshCart();
}

void CartPage::on_btnClose_clicked()
{
    close();
}
