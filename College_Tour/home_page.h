#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class HomePage;
}
QT_END_NAMESPACE

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage();

private:
    Ui::HomePage *ui;
};
#endif // HOME_PAGE_H
