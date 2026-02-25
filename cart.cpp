// #include "cart.h"
// #include "data_manager.h"
// #include "data_types.h"

// #include <QInputDialog>
// #include <QMessageBox>

// CartTester::CartTester(ShoppingCart& cart, DataManager* dm, QWidget* parent)
//     : QObject(parent), m_cart(cart), m_dm(dm), m_parent(parent)
// {
// }

// void CartTester::add_item_prompt()
// {
//     bool ok = false;

//     int college_id = QInputDialog::getInt(
//         m_parent, "Add Souvenir",
//         "Enter college_id:",
//         1, 0, 999999, 1, &ok
//         );
//     if (!ok) return;

//     int souvenir_id = QInputDialog::getInt(
//         m_parent, "Add Souvenir",
//         "Enter souvenir_id:",
//         1, 0, 999999, 1, &ok
//         );
//     if (!ok) return;

//     int qty = QInputDialog::getInt(
//         m_parent, "Add Souvenir",
//         "Enter quantity:",
//         1, 1, 99, 1, &ok
//         );
//     if (!ok) return;

//     // Look up souvenir so we can print what it is + which college it belongs to
//     auto s = m_dm->get_souvenir(souvenir_id);
//     if (!s.has_value())
//     {
//         QMessageBox::warning(m_parent, "Add Failed", "souvenir_id not found in database.");
//         return;
//     }

//     // Optional safety: make sure the user-entered college matches the souvenir's owner
//     if (s->owner_college_id != college_id)
//     {
//         QMessageBox::warning(
//             m_parent, "Add Failed",
//             "That souvenir does not belong to that college.\n"
//             "Try the college_id = " + QString::number(s->owner_college_id)
//             );
//         return;
//     }

//     bool added = m_cart.add_item(m_dm, souvenir_id, qty);
//     if (!added)
//     {
//         QMessageBox::warning(m_parent, "Add Failed", "Could not add item (unexpected).");
//         return;
//     }

//     // Get college name (nice output)
//     QString college_name = "college_id " + QString::number(college_id);
//     auto colleges = m_dm->get_all_colleges();
//     for (const auto& c : colleges)
//     {
//         if (c.college_id == college_id)
//         {
//             college_name = c.name;
//             break;
//         }
//     }

//     QString msg;
//     msg += "Added to cart:\n";
//     msg += "• College: " + college_name + " (id " + QString::number(college_id) + ")\n";
//     msg += "• Souvenir: " + s->name + " (id " + QString::number(s->souvenir_id) + ")\n";
//     msg += "• Qty: " + QString::number(qty) + "\n";
//     msg += "• Unit price: $" + QString::number(s->price, 'f', 2) + "\n";
//     msg += "\nCampus total: $" + QString::number(m_cart.total_cost_for_college(college_id), 'f', 2) + "\n";
//     msg += "Grand total: $" + QString::number(m_cart.grand_total(), 'f', 2);

//     QMessageBox::information(m_parent, "Item Added", msg);
// }

// void CartTester::show_grand_total()
// {
//     QMessageBox::information(
//         m_parent, "Grand Total",
//         "Grand total: $" + QString::number(m_cart.grand_total(), 'f', 2)
//         );
// }
