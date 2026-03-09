#include "trip_overview.h"
#include "ui_trip_overview.h"
#include "data_manager.h"
#include "cart_page.h"
#include <QMessageBox>

trip_overview::trip_overview(ShoppingCart* cart, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::trip_overview)
    , m_cart(cart)
{
    ui->setupUi(this);

    // Style the Header (trip_overview_2)
    ui->trip_overview_2->setStyleSheet(
        "font-size: 42px; "
        "font-weight: bold; "
        "color: #B41E28; "
        "font-family: 'Segoe UI';"
        );

    // Style the Circles
    QString circleStyle =
        "QLabel { "
        "  background-color: #B41E28; "
        "  color: white; "
        "  border-radius: 35px; "
        "  min-width: 70px; "
        "  min-height: 70px; "
        "  max-width: 70px; "
        "  max-height: 70px; "
        "  font-weight: bold; "
        "  font-size: 18px; "
        "  border: 3px solid white; "
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

    // Style the Arrows
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
        if (!word.isEmpty() && word[0].isUpper()) initials += word[0];
    }
    return initials;
}

void trip_overview::loadCurrentLeg()
{
    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int startIndex = DataManager::instance()->get_current_trip_index();

    QList<QLabel*> circles = {ui->circle1, ui->circle2, ui->circle3, ui->circle4, ui->circle5};
    QList<QLabel*> arrows = {ui->label, ui->label_2, ui->label_3, ui->label_4};

    for (int i = 0; i < 5; ++i) {
        int pathIndex = startIndex + i;

        if (pathIndex < fullTrip.size()) {
            int collegeId = fullTrip[pathIndex];
            QString name = DataManager::instance()->get_college_name(collegeId).value_or("");

            circles[i]->setText(getInitials(name));
            circles[i]->show();

            if (i > 0) arrows[i-1]->show();
        } else {
            circles[i]->hide();
            if (i > 0) arrows[i-1]->hide();
        }
    }

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

    if (currentIndex + 5 < fullTrip.size()) {
        DataManager::instance()->set_current_trip_index(currentIndex + 5);
        loadCurrentLeg();
    } else {
        QMessageBox::information(this, "End of Route", "You are viewing the end of the route. Click 'Visit Campuses' to begin traveling!");
    }
}

// --- CHANGE 1: Back Navigation Logic ---
void trip_overview::on_backButton_clicked()
{
    int currentIndex = DataManager::instance()->get_current_trip_index();

    if (currentIndex - 5 >= 0) {
        DataManager::instance()->set_current_trip_index(currentIndex - 5);
        loadCurrentLeg();
    } else {
        QMessageBox::information(this, "Start of Trip", "You are already viewing the beginning of the trip.");
    }
}

void trip_overview::on_visitCampusButton_clicked()
{
    if (!m_cart) return;

    QVector<int> fullTrip = DataManager::instance()->get_current_trip();
    int currentIndex = DataManager::instance()->get_current_trip_index();

    if (currentIndex < 0 || currentIndex >= fullTrip.size()) return;

    int currentCollegeId = fullTrip[currentIndex];

    CartPage dlg(*m_cart, DataManager::instance(), this);
    dlg.openForCollege(currentCollegeId);

    // 1. Hide the map while shopping
    this->hide();

    // 2. Open the cart loop
    dlg.exec();

    // 3. When the cart loop finishes, check if the whole trip is done
    int newIndex = DataManager::instance()->get_current_trip_index();
    if (newIndex >= fullTrip.size()) {
        this->close(); // Close the map permanently (Summary is now showing!)
    } else {
        this->show();  // Show the map again for the next leg
        loadCurrentLeg();
    }
}

void trip_overview::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    int dynamicFontSize = this->width() / 15;

    if (dynamicFontSize < 24) dynamicFontSize = 24;
    if (dynamicFontSize > 80) dynamicFontSize = 80;

    QString dynamicStyle = QString("font-size: %1px; color: #B41E28; font-weight: bold;").arg(dynamicFontSize);

    ui->label->setStyleSheet(dynamicStyle);
    ui->label_2->setStyleSheet(dynamicStyle);
    ui->label_3->setStyleSheet(dynamicStyle);
    ui->label_4->setStyleSheet(dynamicStyle);
}
