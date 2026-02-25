#include "shopping_cart.h"
#include "data_manager.h"

#include <optional>

    // ---------------- add items ----------------

    bool ShoppingCart::add_item(const souvenir& s, int qty)
{
    if (!qty_ok(qty) || s.souvenir_id < 0 || s.owner_college_id < 0)
        return false;

    Item& it = m_items[s.owner_college_id][s.souvenir_id];

    if (it.souvenir_id < 0)
    {
        it.souvenir_id = s.souvenir_id;
        it.college_id = s.owner_college_id;
        it.name = s.name;
        it.unit_price = s.price;
        it.quantity = 0;
    }

    it.quantity += qty;
    return true;
}

bool ShoppingCart::add_item(DataManager* dm, int souvenir_id, int qty)
{
    if (!dm || !qty_ok(qty) || souvenir_id < 0)
        return false;

    std::optional<souvenir> s = dm->get_souvenir(souvenir_id);
    if (!s.has_value())
        return false;

    return add_item(*s, qty);
}

bool ShoppingCart::add_item(DataManager* dm, int college_id, const QString& souvenir_name, int qty)
{
    if (!dm || !qty_ok(qty) || college_id < 0)
        return false;

    std::optional<int> sid = dm->get_souvenir_id(college_id, souvenir_name);
    if (!sid.has_value())
        return false;

    return add_item(dm, *sid, qty);
}

// ---------------- modify/remove ----------------

bool ShoppingCart::set_quantity(int souvenir_id, int qty)
{
    if (souvenir_id < 0)
        return false;

    for (auto college_it = m_items.begin(); college_it != m_items.end(); ++college_it)
    {
        auto& inner = college_it.value();
        auto it = inner.find(souvenir_id);

        if (it == inner.end())
            continue;

        if (qty <= 0)
        {
            inner.erase(it);
            if (inner.isEmpty())
                m_items.erase(college_it);
            return true;
        }

        it->quantity = qty;
        return true;
    }

    return false;
}

bool ShoppingCart::remove(int souvenir_id)
{
    return set_quantity(souvenir_id, 0);
}

void ShoppingCart::clear()
{
    m_items.clear();
}

// ---------------- queries ----------------

bool ShoppingCart::contains(int souvenir_id) const
{
    for (auto college_it = m_items.constBegin(); college_it != m_items.constEnd(); ++college_it)
    {
        if (college_it.value().contains(souvenir_id))
            return true;
    }
    return false;
}

int ShoppingCart::quantity(int souvenir_id) const
{
    for (auto college_it = m_items.constBegin(); college_it != m_items.constEnd(); ++college_it)
    {
        const auto& inner = college_it.value();
        auto it = inner.find(souvenir_id);
        if (it != inner.end())
            return it->quantity;
    }
    return 0;
}

QVector<ShoppingCart::Item> ShoppingCart::items_for_college(int college_id) const
{
    QVector<Item> out;

    if (!m_items.contains(college_id))
        return out;

    const auto& inner = m_items[college_id];

    for (auto it = inner.constBegin(); it != inner.constEnd(); ++it)
        out.push_back(it.value());

    return out;
}

QVector<ShoppingCart::Item> ShoppingCart::all_items() const
{
    QVector<Item> out;

    for (auto college_it = m_items.constBegin(); college_it != m_items.constEnd(); ++college_it)
    {
        const auto& inner = college_it.value();
        for (auto it = inner.constBegin(); it != inner.constEnd(); ++it)
            out.push_back(it.value());
    }

    return out;
}

int ShoppingCart::total_items_for_college(int college_id) const
{
    int total = 0;
    for (const Item& it : items_for_college(college_id))
        total += it.quantity;
    return total;
}

double ShoppingCart::total_cost_for_college(int college_id) const
{
    double total = 0.0;
    for (const Item& it : items_for_college(college_id))
        total += it.unit_price * it.quantity;
    return total;
}

double ShoppingCart::grand_total() const
{
    double total = 0.0;

    for (auto college_it = m_items.constBegin(); college_it != m_items.constEnd(); ++college_it)
    {
        const auto& inner = college_it.value();
        for (auto it = inner.constBegin(); it != inner.constEnd(); ++it)
            total += it.value().unit_price * it.value().quantity;
    }

    return total;
}
