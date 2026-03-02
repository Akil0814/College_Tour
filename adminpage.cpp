#include "adminpage.h"
#include "ui_adminpage.h"
#include "data_manager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

adminpage::adminpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminpage)
{
    ui->setupUi(this);

    populateColleges();
    populateEditTypes();
}

adminpage::~adminpage()
{
    delete ui;
}

void adminpage::populateColleges()
{
    // Get the list of all colleges from the DataManager
    QVector<college> colleges = DataManager::instance()->get_all_colleges();

    // Clear existing items to avoid duplicates if called multiple times
    ui->schoolSelectDD->clear();

    // Iterate through the database results and add names to dropdowns
    for (const auto& col : colleges) {
        ui->schoolSelectDD->addItem(col.name, col.college_id);
    }
}

void adminpage::populateEditTypes()
{
    ui->editTypeDD->addItem("Edit Existing Souvenir");
    ui->editTypeDD->addItem("Add New Souvenir");
}

// Upon edit type being changed update remove and item input fields
void adminpage::on_editTypeDD_currentIndexChanged(int index)
{
    // Get user selection
    QString selected = ui->editTypeDD->itemText(index);

    // Switch stacked widgets depending on edit type
    if (selected == "Edit Existing Souvenir") {
        ui->itemDDTextSwitch->setCurrentIndex(0);
        ui->removeButton->setEnabled(true);

        // Done to force updated price and item list when switching from add to edit
        on_schoolSelectDD_currentIndexChanged(ui->schoolSelectDD->currentIndex());
        // on_itemDD_currentIndexChanged(ui->itemDD->currentIndex());
    }
    else if (selected == "Add New Souvenir") {
        ui->itemDDTextSwitch->setCurrentIndex(1);
        ui->removeButton->setEnabled(false);
        ui->priceLineEdit->clear();
        ui->itemLineEdit->clear();
    }
}

// Upon changing selected school update fields
void adminpage::on_schoolSelectDD_currentIndexChanged(int index)
{
    // Clear souviner list
    ui->itemDD->clear();

    // Get data and translate it to string
    QVariant collegeData = ui->schoolSelectDD->currentText();
    QString collegeName = collegeData.toString();

    // Get souviners for selected college
    QVector <souvenir> souvenirs = DataManager::instance()->get_all_souvenirs_from_college(collegeName);

    // qDebug() << "Selected college:" << collegeName;

    // Populate item dropdown
    for (const auto& s : souvenirs)
    {
        ui->itemDD->addItem(s.name, s.souvenir_id);
    }

    // Clear price and custom item name fields if in add new edit
    if (ui->editTypeDD->currentText() == "Add New Souvenir")
    {
        ui->itemLineEdit->clear();
        ui->priceLineEdit->clear();
    }
}

// Upon item dropdown index changing (Changing schools / changing items)
void adminpage::on_itemDD_currentIndexChanged(int index)
{
    // Retrieve souvenir by id
    int souvenirId = ui->itemDD->currentData().toInt();
    std::optional <souvenir> s = DataManager::instance()->get_souvenir(souvenirId);

    // Set price field if value exists
    if (s.has_value())
    {
        // Set price field | convert to string, print floating point to 2 places
        ui->priceLineEdit->setText(QString::number(s->price, 'f', 2));
    }
    else
    {
        ui->priceLineEdit->clear();
    }
}

void adminpage::on_removeButton_clicked()
{
    // Retrieve souvenir to remove
    int souvenirId = ui->itemDD->currentData().toInt();

    // Remove souvenir
    bool successful = DataManager::instance()->delete_souvenir(souvenirId);

    if (!successful)
    {
        qDebug() << "Item not removed";
    }

    // Update item dropdown to reflect changes
    on_schoolSelectDD_currentIndexChanged(ui->schoolSelectDD->currentIndex());
}

// Saves changes currently displayed | alternate flow for existing and new item
void adminpage::on_saveChangesButton_clicked()
{
    // Existing item save button workflow
    if (ui->editTypeDD->currentText() == "Edit Existing Souvenir")
    {
        // Get souvenir id
        int id = ui->itemDD->currentData().toInt();

        // Get validated price
        std::optional<double>price = check_price(ui->priceLineEdit->text());

        // Output error and early out for invalid price
        if (!price.has_value())
        {
            QMessageBox::warning(this, "Invalid Price", "Enter a Valid Price.");
            return;
        }
        // Update item price and output error if adjustment failed
        if (!DataManager::instance()->adjust_souvenir_price(id, price.value()))
        {
            QMessageBox::warning(this, "Update Unsuccessful", "Failed to Update Souvenir Price.");
        }
    }
    // Add new item save button workflow
    else
    {
        // Get new souvenir data | name, price, school affiliation
        QString name = ui->itemLineEdit->text();
        std::optional<double>price = check_price(ui->priceLineEdit->text());
        int collegeId = ui->schoolSelectDD->currentData().toInt();

        // Output error message and early out for invalid input
        if (name.isEmpty())
        {
            QMessageBox::warning(this, "Invalid Name", "Name Cannot Be Empty");
            return;
        }
        if(!price.has_value())
        {
            QMessageBox::warning(this, "Invalid Price", "Enter a Valid Price.");
            return;
        }

        // Check for duplicate
        // Note this is done because a duplicate to database returns id of existing item not null
        std::optional<int> exists = DataManager::instance()->get_souvenir_id(collegeId, name);
        if (exists.has_value())
        {
            // Get college name for error message or use default name
            std::optional<QString> collegeOptional = DataManager::instance()->get_college_name(collegeId);
            QString collegeName = collegeOptional.value_or("This College.");

            QMessageBox::warning(this, "Duplicate Souvenir", "Item With This Name Already Exists at " + collegeName);
            return;
        }


        // Create souvenir and add to database
        souvenir s;
        s.name = name;
        s.price = price.value();
        s.owner_college_id = collegeId;

        std::optional<int> added = DataManager::instance()->add_souvenir(s);

        // If not added output error message from database
        if (!added.has_value())
        {
            QString error = DataManager::instance()->last_error();
            QMessageBox::warning(this, "Souvenir Not Added", error);
        }
    }
}

// Function takes in a price as string, checks if its valid, and returns as a double/null
std::optional<double> adminpage::check_price(QString price_text)
{
    bool converted;
    double price = price_text.toDouble(&converted);

    if (!converted || price < 0)
    {
        return std::nullopt;
    }
    return price;
}

// Start file selection sequence
void adminpage::on_editInputFileButton_clicked()
{
    // Start file selection sequence and preferably end with path of chosen file
    QString path = QFileDialog::getOpenFileName(this, "Select File", QDir::homePath(), "CSV FILES (*.csv);; All Files (*)");

    // Check if user canceled file selection
    if (path.isEmpty())
    {
        return;
    }

    // Update database with new file
    QFileInfo info(path);
    DataManager::instance()->add_campus_from_file(path, info.fileName());
}


void adminpage::on_resetButton_clicked()
{
    // UPDATE functionality with safety around reset

    // Reset database
    bool reset = DataManager::instance()->reset_database();
    qDebug() << "Reset " << reset;
}

// Remove currently selected university
void adminpage::on_removeSchoolButton_clicked()
{
    // Get college to remove
    int collegeId = ui->schoolSelectDD->currentData().toInt();

    // Call database to remove school
    bool deleted = DataManager::instance()->delete_college(collegeId);

    // Error message
    if (!deleted)
    {
        QString error = DataManager::instance()->last_error();
        QMessageBox::warning(this, "University Deletion Unsuccessful", error);
        return;
    }

    // Reset college list
    populateColleges();
}

