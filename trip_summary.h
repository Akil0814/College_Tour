#ifndef TRIP_SUMMARY_H
#define TRIP_SUMMARY_H

#include <QWidget>
#include "data_manager.h"
#include "shopping_cart.h"
#include "distance_tracker.h"

struct SummaryLine
{
    QString location;
    int numPurchased;
    double locationCost;
};

// Class locally saves most recent summary table for retreival
struct SummaryData
{
    QVector<SummaryLine> tableData;
    int totalLocations;
    int totalNumPurchased;
    double totalSpent;
    int totalDistance;
};

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

    // Returns most recent table and stats created
    inline SummaryData getSummaryData() {return summaryData;}

private slots:
    void on_exitSummaryButton_clicked();

    void on_saveSummaryButton_clicked();

private:
    Ui::TripSummary *ui;

    SummaryData summaryData;

    // Called in populateTable, updates stats section with totals
    void populateStats(QVector<int>& locations, ShoppingCart *cart, DistanceTracker *dt);

    // Clears stored summary table data
    void clearSummaryCache();
};

#endif // TRIP_SUMMARY_H
