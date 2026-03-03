#include "home_page.h"
#include "data_manager.h"
#include "cart.h"
#include "./ui_homepage.h"
#include "planatrip.h"
#include "adminpage.h"
#include "login_window.h"

#include "login_window.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent), ui(new Ui::HomePage) {

    // 1. Setup the Background Image
    heroImage = new QLabel(this);
    // Correct path: prefix + folder + filename
    heroImage->setPixmap(QPixmap(":/res/res/saddleback-college-gateway-building-1050x750-compact.png"));
    heroImage->setScaledContents(true);
    heroImage->lower(); // Keep it behind the sidebar

    // 2. Main Layout (Seamless)
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 3. Rigid Sidebar Frame
    sidebarFrame = new QFrame(this);
    sidebarFrame->setObjectName("sidebarFrame");
    sidebarFrame->setFixedWidth(450);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarFrame);
    sidebarLayout->setContentsMargins(50, 60, 50, 60);
    sidebarLayout->setSpacing(15);

    // Logo with correct path
    QLabel *logo = new QLabel();
    logo->setPixmap(QPixmap(":/res/res/Saddleback.png").scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    titleLabel = new QLabel("CAMPUS NAVIGATOR:\nExplore Your Future");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setWordWrap(true);

    subHeadingLabel = new QLabel("Discover academic paths and campus life.");
    subHeadingLabel->setObjectName("subHeadingLabel");

    studentButton = new QPushButton("STUDENT PORTAL");
    adminButton = new QPushButton("ADMIN LOGIN");




    // Add elements with spacing
    sidebarLayout->addWidget(logo);
    sidebarLayout->addSpacing(40);
    sidebarLayout->addWidget(titleLabel);
    sidebarLayout->addWidget(subHeadingLabel);
    sidebarLayout->addStretch(1);
    sidebarLayout->addWidget(studentButton);
    sidebarLayout->addWidget(adminButton);

    mainLayout->addWidget(sidebarFrame);
    mainLayout->addStretch(1);

    // 4. STYLING: Lower opacity (180) to make the transparency obvious
    this->setStyleSheet(
        "QWidget#HomePage { background-color: transparent; }"
        "QFrame#sidebarFrame { background-color: rgba(180, 30, 40, 180); border: none; }"
        "QLabel#titleLabel { color: white; font-size: 36px; font-weight: bold; font-family: 'Segoe UI'; }"
        "QLabel#subHeadingLabel { color: rgba(255, 255, 255, 0.7); font-size: 16px; }"
        "QPushButton { background-color: white; border: none; border-radius: 25px; color: #B41E28; "
        "font-size: 15px; font-weight: bold; padding: 15px; }"
        "QPushButton:hover { background-color: #eeeeee; }"
        );

    connect(studentButton, &QPushButton::clicked, this, &HomePage::on_student_button_clicked);
    connect(adminButton, &QPushButton::clicked, this, &HomePage::on_admin_button_clicked);

    // m_cartTester = new CartTester(m_cart, DataManager::instance(), this);


    // connect(grandTotalButton, &QPushButton::clicked, m_cartTester, &CartTester::show_grand_total); //added
    // connect(addItemButton, &QPushButton::clicked,
    //      this, &HomePage::on_add_item_button_clicked);//added

    //added

}
HomePage::~HomePage() { delete ui; }

// Keeps the background image stretched to the window size
void HomePage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if(heroImage) {
        heroImage->setGeometry(0, 0, this->width(), this->height());
    }
}

void HomePage::on_student_button_clicked() {
    // Direct routing for students to the trip planner
    PlanATrip *tripWindow = new PlanATrip(&m_cart, this); //added
    tripWindow->setAttribute(Qt::WA_DeleteOnClose);
    tripWindow->setWindowModality(Qt::ApplicationModal);
    tripWindow->show();
}

void HomePage::on_admin_button_clicked() {
    // Open the login window and wait for the user
    login_window login(this);

    // If id_verify succeeds, the login window calls this->accept()
    if (login.exec() == QDialog::Accepted) {

        // Login was successful! Open the Admin Page
        adminpage *adminWindow = new adminpage();
        adminWindow->setAttribute(Qt::WA_DeleteOnClose);
        adminWindow->show();
    }
}


//added
void HomePage::on_add_item_button_clicked()
{
    auto colleges = DataManager::instance()->get_all_colleges();

    for (const auto& c : colleges)
    {
        qInfo() << "College:"
                << c.name
                << "(ID:" << c.college_id << ")";

        auto souvenirs =
            DataManager::instance()->get_all_souvenirs_from_college(c.college_id);

        for (const auto& s : souvenirs)
        {
            qInfo() << "   -" << s.name
                    << "(Souvenir ID:" << s.souvenir_id
                    << ", $" << s.price << ")";
        }
    }
    // m_cartTester->add_item_prompt();
}

