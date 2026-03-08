#include "trip_overview.h"
#include "ui_trip_overview.h"
#include "data_manager.h"
#include <QMessageBox>

trip_overview::trip_overview(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::trip_overview)
{
    ui->setupUi(this);

    // 1. Style the Header (trip_overview_2)
    ui->trip_overview_2->setStyleSheet(
        "font-size: 42px; "
        "font-weight: bold; "
        "color: #B41E28; " // More red
        "font-family: 'Segoe UI';"
        );

    // --- NEW STYLE: Bigger and Redder ---
    QString circleStyle =
        "QLabel { "
        "  background-color: #B41E28; " // Saddleback Red
        "  color: white; "
        "  border-radius: 35px; "      // Half of 70px makes it a circle
        "  min-width: 70px; "
        "  min-height: 70px; "
        "  max-width: 70px; "
        "  max-height: 70px; "
        "  font-weight: bold; "
        "  font-size: 18px; "          // Larger font for initials
        "  border: 3px solid white; "  // Makes the red pop more
        "}";

    ui->circle1->setStyleSheet(circleStyle);
    ui->circle2->setStyleSheet(circleStyle);
    ui->circle3->setStyleSheet(circleStyle);
    ui->circle4->setStyleSheet(circleStyle);
    ui->circle5->setStyleSheet(circleStyle);

    ui->circle1->setAlignment(Qt::AlignCenter);
    ui->circle2->setAlignment(Qt::AlignCenter);
    ui->circle3->setAlignment(Qt::AlignCenter);
    ui->circle4->setAlignment(Qt::AlignCenter);
    ui->circle5->setAlignment(Qt::AlignCenter);

    QString arrowStyle = "font-size: 40px; color: #B41E28; font-weight: bold;";
    ui->label->setStyleSheet(arrowStyle);
    ui->label_2->setStyleSheet(arrowStyle);
    ui->label_3->setStyleSheet(arrowStyle);
    ui->label_4->setStyleSheet(arrowStyle);

    ui->label->setText("→");
    ui->label_2->setText("→");
    ui->label_3->setText("→");
    ui->label_4->setText("→");

    loadCurrentLeg();
}

trip_overview::~trip_overview()
{
    delete ui;
}

static QString getInitials(QString name) {
    QString initials;
    for (const QString &word : name.split(" ")) {
        // Grab the first letter if it's uppercase (e.g. "Arizona State University" -> "ASU")
        if (!word.isEmpty() && word[0].isUpper()) initials += word[0];
    }
    return initials;
}

void trip_overview::loadCurrentLeg()
{
    // 1. Get the full route and current progress
    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int startIndex = DataManager::instance()->get_current_trip_index();

    // 2. Map UI elements into arrays for easy looping
    QList<QLabel*> circles = {ui->circle1, ui->circle2, ui->circle3, ui->circle4, ui->circle5};
    QList<QLabel*> arrows = {ui->label, ui->label_2, ui->label_3, ui->label_4};

    // 3. Loop strictly through the optimized sequence
    for (int i = 0; i < 5; ++i) {
        int pathIndex = startIndex + i; // The exact position in the optimized route

        if (pathIndex < fullTrip.size()) {
            int collegeId = fullTrip[pathIndex];

            // Fetch the exact name for this specific ID one at a time to preserve order
            QString name = DataManager::instance()->get_college_name(collegeId).value_or("");

            circles[i]->setText(getInitials(name));
            circles[i]->show();

            // Show arrow if it's connecting to a visible node
            if (i > 0) arrows[i-1]->show();
        } else {
            // Hide unused circles and arrows
            circles[i]->hide();
            if (i > 0) arrows[i-1]->hide();
        }
    }

    // 4. Update button text
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
        QMessageBox::information(this, "End of Trip", "You have finished your college tour!");
        this->close();
    } else {
        // Advance by 5 for the next leg
        DataManager::instance()->set_current_trip_index(currentIndex + 5);
        loadCurrentLeg();
    }
}

// --- ADD THIS: Dynamic Resizing Logic at the bottom ---
void trip_overview::resizeEvent(QResizeEvent *event)
{
    // 1. Call the default resize behavior first
    QDialog::resizeEvent(event);

    // 2. Calculate a new font size based on the window's width
    // Dividing by 15 is a good starting ratio, but you can tweak this!
    int dynamicFontSize = this->width() / 15;

    // 3. Set min and max limits so the arrows don't disappear or get comically large
    if (dynamicFontSize < 24) dynamicFontSize = 24;
    if (dynamicFontSize > 80) dynamicFontSize = 80;

    // 4. Create the new stylesheet with the calculated size
    QString dynamicStyle = QString("font-size: %1px; color: #B41E28; font-weight: bold;").arg(dynamicFontSize);

    // 5. Apply the new style to all arrows
    ui->label->setStyleSheet(dynamicStyle);
    ui->label_2->setStyleSheet(dynamicStyle);
    ui->label_3->setStyleSheet(dynamicStyle);
    ui->label_4->setStyleSheet(dynamicStyle);
}
