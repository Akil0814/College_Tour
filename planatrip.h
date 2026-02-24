#ifndef PLANATRIP_H
#define PLANATRIP_H

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
    Ui::PlanATrip *ui;
};

#endif // PLANATRIP_H
