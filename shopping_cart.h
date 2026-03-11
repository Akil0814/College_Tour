#ifndef SHOPPING_CART_H
#define SHOPPING_CART_H

#include <QHash>
#include <QVector>
#include <QString>

#include "data_types.h"
class DataManager;

class ShoppingCart
{
public:
    struct Item
    {
        int souvenir_id = -1;
        int college_id  = -1;
        QString name;
        double unit_price = 0.0;
        int quantity = 0;
    };

public:
    // add items (3 ways)
    bool add_item(const souvenir& s, int qty = 1);
    bool add_item(DataManager* dm, int souvenir_id, int qty = 1);
    bool add_item(DataManager* dm, int college_id, const QString& souvenir_name, int qty = 1);

    // modify/remove
    bool set_quantity(int souvenir_id, int qty);
    bool remove(int souvenir_id);
    void clear();

    // allowing quick access for these variables
    bool contains(int souvenir_id) const;
    int quantity(int souvenir_id) const;

    QVector<Item> items_for_college(int college_id) const;
    QVector<Item> all_items() const;

    int total_items_for_college(int college_id) const;
    double total_cost_for_college(int college_id) const;
    double grand_total() const;

private:
    //hash table, using an outer and inner map
    QHash<int, QHash<int, Item>> m_items;

    static bool qty_ok(int qty) { return qty > 0; }
};

#endif
