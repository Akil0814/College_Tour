#ifndef DISTANCES_PAGE_H
#define DISTANCES_PAGE_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class DistancesPage : public QDialog {
    Q_OBJECT

public:
    explicit DistancesPage(QWidget *parent = nullptr);

private slots:
    void onCloseClicked();

private:
    QTableWidget *tableWidget;
};

#endif // DISTANCES_PAGE_H
