#ifndef PLANATRIP_H
#define PLANATRIP_H
#include "data_manager.h" // Add this include

#include <QMainWindow>

namespace Ui {
class PlanATrip;
}

class PlanATrip : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlanATrip(QWidget *parent = nullptr);
    ~PlanATrip();

private:
    // Helper function to load data from database
    void populateColleges();

    Ui::PlanATrip *ui;
};

#endif // PLANATRIP_H
