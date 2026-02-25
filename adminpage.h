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

private:
    Ui::adminpage *ui;
};

#endif // ADMINPAGE_H

