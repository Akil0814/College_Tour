#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QDialog>
#include <QLineEdit> // Added for member variables
#include <QPushButton>

namespace Ui {
class login_window;
}

class login_window : public QDialog
{
    Q_OBJECT

public:
    explicit login_window(QWidget *parent = nullptr);
    ~login_window();

private slots:
    void onLoginClicked(); // The new function to handle verification

private:
    Ui::login_window *ui;
    // Move pointers here so we can access them in onLoginClicked()
    QLineEdit *usernameField;
    QLineEdit *passwordField;
    QPushButton *loginBtn;
};

#endif // LOGIN_WINDOW_H
