#include "login_window.h"
#include "ui_login_window.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

login_window::login_window(QWidget *parent)
    : QDialog(parent), ui(new Ui::login_window) {


    this->setWindowTitle("Campus Access");
    this->setFixedSize(320, 350);

    // 2. Main Layout with tight spacing
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    // Theme Header
    QLabel *header = new QLabel("Member Login");
    header->setStyleSheet("font-size: 22px; font-weight: bold; color: #B41E28;");
    header->setAlignment(Qt::AlignCenter);

    QLineEdit *username = new QLineEdit();
    username->setPlaceholderText("Username");

    QLineEdit *password = new QLineEdit();
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);

    // 3. The Themed Login Button
    QPushButton *loginBtn = new QPushButton("LOGIN");
    loginBtn->setObjectName("loginBtn");
    loginBtn->setCursor(Qt::PointingHandCursor);

    // Assemble
    layout->addWidget(header);
    layout->addSpacing(10);
    layout->addWidget(username);
    layout->addWidget(password);
    layout->addStretch(1);
    layout->addWidget(loginBtn);

    // 4. Styling to match HomePage (Opacity and Colors)
    this->setStyleSheet(
        "QDialog { background-color: #ffffff; border-radius: 10px; }"
        "QLineEdit { "
        "   padding: 12px; " // Extra padding fixes the text being cut off
        "   border: 2px solid #f2f2f2; "
        "   border-radius: 8px; "
        "   background: #fdfdfd; "
        "   color: #333; "
        "   font-size: 14px; "
        "}"
        "QLineEdit:focus { border: 2px solid #B41E28; }"
        "QPushButton#loginBtn { "
        "   background-color: #B41E28; " // Matches the Homepage Red
        "   color: white; "
        "   border-radius: 20px; "
        "   padding: 12px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton#loginBtn:hover { background-color: #961921; }"
        );

    connect(loginBtn, &QPushButton::clicked, this, &QDialog::accept);
}

login_window::~login_window() {
    delete ui;
}
