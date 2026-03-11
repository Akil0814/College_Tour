#ifndef TRIP_SUMMARY_H
#define TRIP_SUMMARY_H

#include <QWidget>
#include "data_manager.h"
#include "shopping_cart.h"
#include "distance_tracker.h"
#include "home_page.h"
#include "cart_page.h"

namespace Ui {
class TripSummary;
}

class TripSummary : public QWidget
{
    Q_OBJECT

public:
    explicit TripSummary(QWidget *parent = nullptr);
    ~TripSummary();

    // Must be called before showing window
    void populateTable(ShoppingCart *cart, DistanceTracker *dt);

private slots:
    void on_exitSummaryButton_clicked();

private:
    Ui::TripSummary *ui;
    ShoppingCart* m_cart = nullptr;

    // Called in populateTable, updates stats section with totals
    void populateStats(QVector<int>& locations, ShoppingCart *cart, DistanceTracker *dt);
};

#endif // TRIP_SUMMARY_H
