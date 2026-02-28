#ifndef PLANATRIP_H
#define PLANATRIP_H
#include "data_manager.h" // Add this include

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class PlanATrip;
}

class PlanATrip : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *bg;

public:
    explicit PlanATrip(QWidget *parent = nullptr);
    ~PlanATrip();

private slots:
    void on_addStopButton_clicked(); // Slot for the "+" button
    void on_goButton_clicked();      // Slot to start optimization
    // This function triggers whenever the user selects a school from the list
    void on_tripStopsDropDown_activated(int index);

private:
    void populateColleges();

    Ui::PlanATrip *ui;
    int startingCollegeId;
    QVector<int> tripStops;          // The list of stops to optimize


protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // PLANATRIP_H
