#include "trip_overview.h"
#include "ui_trip_overview.h"
#include "data_manager.h"
#include <QMessageBox>

trip_overview::trip_overview(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::trip_overview)
{
    ui->setupUi(this);

    loadCurrentLeg();
}

trip_overview::~trip_overview()
{
    delete ui;
}

void trip_overview::loadCurrentLeg()
{
    // 1. Get the full route and our starting point for this leg
    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int startIndex = DataManager::instance()->get_current_trip_index();

    // 2. Put the UI circles into a list so we can loop through them
    QList<QLabel*> circles = {ui->circle1, ui->circle2, ui->circle3, ui->circle4, ui->circle5};

    // 3. Extract just the IDs for the up-to-5 colleges in this specific leg
    QVector<int> currentLegIds;
    for (int i = 0; i < 5; ++i) {
        int index = startIndex + i;
        if (index < fullTrip.size()) {
            currentLegIds.append(fullTrip[index]);
        }
    }

    // 4. Use your existing DataManager function to get the initials!
    QVector<QString> initials = DataManager::instance()->get_initials(currentLegIds);

    // 5. Update the UI
    for (int i = 0; i < 5; ++i) {
        if (i < initials.size()) {
            circles[i]->setText(initials[i]);
            circles[i]->show(); // Make sure it is visible
        } else {
            circles[i]->hide(); // Hide the circle if we have fewer than 5 stops left
        }
    }

    // 6. Optional UI polish: Change the button text if we are at the end
    if (startIndex + 5 >= fullTrip.size()) {
        ui->nextButton->setText("Finish Trip");
    } else {
        ui->nextButton->setText("Start Next Leg");
    }
}

void trip_overview::on_nextButton_clicked()
{
    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int currentIndex = DataManager::instance()->get_current_trip_index();

    if (currentIndex + 5 >= fullTrip.size()) {
        // The whole trip is over!
        QMessageBox::information(this, "End of Trip", "You have finished your college tour!");
        this->close();
    } else {
        // --- PASSING THE BATON ---
        // Right here is where you will eventually launch your teammate's
        // 5 college-specific pages!

        // For testing purposes right now, let's just jump the index forward
        // by 5 and reload the UI to prove the chunking works:
        DataManager::instance()->set_current_trip_index(currentIndex + 5);
        loadCurrentLeg();
    }
}
