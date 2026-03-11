#ifndef CART_PAGE_H
#define CART_PAGE_H

#include <QDialog>
#include "shopping_cart.h"

class DataManager;

namespace Ui {
class CartPage;
}

class CartPage : public QDialog
{
    Q_OBJECT

public:
    explicit CartPage(ShoppingCart& cart, DataManager* dm, QWidget* parent = nullptr);
    ~CartPage();

    void openForCollege(int college_id);

private slots:
    void on_btnAdd_clicked();
    void on_btnRemove_clicked();
    void on_btnClose_clicked();
     void resizeEvent(QResizeEvent *event) override;

private:
    void refreshSouvenirs();
    void refreshCart();
    void refreshHeader();

    Ui::CartPage *ui;
    ShoppingCart& m_cart;
    DataManager* m_dm;
    int m_college_id = -1;
};

#endif // CART_PAGE_H
