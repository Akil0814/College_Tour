#include "distances_page.h"
#include "data_manager.h"
#include <QHeaderView>

DistancesPage::DistancesPage(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Distances from Saddleback College");
    resize(600, 700);

    setStyleSheet(
        "QDialog { background-color: #f5f5f5; }"
        "QLabel { color: #B41E28; font-size: 28px; font-weight: bold; font-family: 'Segoe UI'; margin-bottom: 10px; }"
        "QTableWidget { background-color: white; font-size: 16px; border: 2px solid #B41E28; gridline-color: #dddddd; }"
        "QHeaderView::section { background-color: #B41E28; color: white; font-weight: bold; font-size: 16px; padding: 5px; }"
        "QPushButton { background-color: #B41E28; color: white; font-size: 16px; font-weight: bold; border-radius: 20px; padding: 10px; min-width: 150px; }"
        "QPushButton:hover { background-color: #8B151E; }"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *titleLabel = new QLabel("Distances from Saddleback");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "College Campus" << "Distance (Miles)");
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    layout->addWidget(tableWidget);

    QPushButton *closeButton = new QPushButton("Close", this);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
    connect(closeButton, &QPushButton::clicked, this, &DistancesPage::onCloseClicked);

    auto colleges = DataManager::instance()->get_all_colleges();
    int row = 0;

    for (const auto& c : colleges) {
        if (c.name == "Saddleback College") continue; // Skip measuring Saddleback to Saddleback

        // Check distance (handling the directional check we fixed earlier!)
        auto distOpt = DataManager::instance()->get_distance_between_college("Saddleback College", c.name);
        if (!distOpt.has_value()) {
            distOpt = DataManager::instance()->get_distance_between_college(c.name, "Saddleback College");
        }

        // If an edge exists in the DB, add it to the chart
        if (distOpt.has_value()) {
            tableWidget->insertRow(row);
            tableWidget->setItem(row, 0, new QTableWidgetItem(c.name));

            QTableWidgetItem *milesItem = new QTableWidgetItem(QString::number(distOpt.value(), 'f', 1));
            milesItem->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, 1, milesItem);

            row++;
        }
    }
}

void DistancesPage::onCloseClicked() {
    this->close();
}
