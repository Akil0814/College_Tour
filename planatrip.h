#ifndef PLANATRIP_H
#define PLANATRIP_H

#include <QMainWindow>
#include <QLabel>
#include "shopping_cart.h"

// FIX: Add this line so the compiler knows what a QListWidgetItem is
class QListWidgetItem;

namespace Ui {
class PlanATrip;
}

class PlanATrip : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *bg;

public:
    explicit PlanATrip(ShoppingCart* m_cart, QWidget *parent = nullptr);
    ~PlanATrip();

private slots:
    void on_goButton_clicked();      // Slot to start optimization
    // This function triggers whenever the user selects a school from the list
    void on_tripStopsDropDown_activated(int index);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
private:
    void populateColleges();

    Ui::PlanATrip *ui;
    int startingCollegeId;
    QVector<int> tripStops;          // The list of stops to optimize

    ShoppingCart* m_cart = nullptr;



protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PLANATRIP_H
