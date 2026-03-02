#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include <QWidget>

namespace Ui {
class adminpage;
}

class adminpage : public QWidget
{
    Q_OBJECT

public:
    explicit adminpage(QWidget *parent = nullptr);
    ~adminpage();

private slots:
    void on_editTypeDD_currentIndexChanged(int index);

    void on_schoolSelectDD_currentIndexChanged(int index);

    void on_itemDD_currentIndexChanged(int index);

    void on_removeButton_clicked();

    void on_saveChangesButton_clicked();

    void on_editInputFileButton_clicked();

    void on_resetButton_clicked();

    void on_removeSchoolButton_clicked();

private:
    Ui::adminpage *ui;
    void populateColleges();
    void populateEditTypes();
    std::optional<double> check_price(QString price_text);
};

#endif // ADMINPAGE_H

