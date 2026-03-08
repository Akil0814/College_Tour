#ifndef TRIP_OVERVIEW_H
#define TRIP_OVERVIEW_H

#include <QDialog>
#include <qlabel.h>
#include <QLabel>
#include <QVector>

namespace Ui {
class trip_overview;
}

class trip_overview : public QDialog
{
    Q_OBJECT

public:
    explicit trip_overview(QWidget *parent = nullptr);
    ~trip_overview();

private slots:
    void on_nextButton_clicked();


protected:
    // --- ADDED THIS: Function to detect window resizing ---
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::trip_overview *ui;

    QVector<QLabel*> nodes;
    QVector<QLabel*> arrows;

    // Helper function to draw the 5 circles
    void loadCurrentLeg();

};

#endif // TRIP_OVERVIEW_H
