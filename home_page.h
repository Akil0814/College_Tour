#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QResizeEvent>
#include "login_window.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class HomePage : public QWidget {
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage(); // This is the destructor the linker is missing

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // These must match the names in your .cpp file exactly
    void on_student_button_clicked();
    void on_admin_button_clicked();

private:
    Ui::HomePage *ui;
    QFrame *sidebarFrame;
    QLabel *heroImage;
    QLabel *titleLabel;
    QLabel *subHeadingLabel;
    QPushButton *studentButton;
    QPushButton *adminButton;
};
#endif
