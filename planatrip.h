#ifndef PLANATRIP_H
#define PLANATRIP_H

#include <QMainWindow>
#include <QLabel>
#include <QVector>

class ShoppingCart;
class QListWidgetItem;
class QResizeEvent;

namespace Ui {
class PlanATrip;
}

class PlanATrip : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlanATrip(ShoppingCart* cart, QWidget* parent = nullptr);
    ~PlanATrip();

private slots:
    void on_goButton_clicked();
    void on_tripStopsDropDown_activated(int index);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_startingPointDropDown_activated(int index);
    void on_premadeTripsDropDown_activated(int index);

private:
    void populateColleges();

    QLabel *bg;
    Ui::PlanATrip *ui;
    int startingCollegeId = -1;
    QVector<int> tripStops;
    ShoppingCart* m_cart = nullptr;

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PLANATRIP_H
