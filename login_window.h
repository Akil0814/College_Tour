#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QDialog>

namespace Ui {
class login_window;
}

class login_window : public QDialog
{
    Q_OBJECT

public:
    explicit login_window(QWidget *parent = nullptr);
    ~login_window();

private:
    Ui::login_window *ui;
};

#endif // LOGIN_WINDOW_H
