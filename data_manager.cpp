#include "data_manager.h"

static bool exec_sql(QSqlQuery& q, QString& last_error, const QString& sql)
{
    if (!q.exec(sql)) {
        last_error = q.lastError().text();
        return false;
    }
    return true;
}

DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

bool DataManager::init()
{
    if (m_init_ed)
        return true;

    if (!open_db())
        return false;

    if (!init_pragmas())
        return false;

    if (!init_schema())
        return false;

    if (!seed_if_empty())
        return false;

    m_init_ed = true;

    return true;
}


bool DataManager::open_db()
{
    std::cout << "1-1.Opening the database" << std::endl;
    m_last_error.clear();

    QDir dir(QDir::current());

    QString data_dir_path;
    for (int i = 0; i < 8; ++i)
    {
        const QString candidate = dir.filePath("data");
        if (QDir(candidate).exists())
        {
            data_dir_path = QDir(candidate).absolutePath();
            break;
        }

        const QString candidate2 = dir.filePath("College_Tour/data");
        if (QDir(candidate2).exists())
        {
            data_dir_path = QDir(candidate2).absolutePath();
            break;
        }

        if (!dir.cdUp())
            break;
    }

    if (data_dir_path.isEmpty())
    {
        const QString fallback = QDir::current().filePath("data");
        QDir().mkpath(fallback);
        data_dir_path = QDir(fallback).absolutePath();
    }

    m_db_path = QDir(data_dir_path).filePath("college_tour.db");

    QSqlDatabase db = QSqlDatabase::contains(m_conn_name)
        ? QSqlDatabase::database(m_conn_name)
        : QSqlDatabase::addDatabase("QSQLITE", m_conn_name);

    db.setDatabaseName(m_db_path);

    if (!db.open())
    {
        m_last_error = db.lastError().text();
        return false;
    }

    std::cout << "1-2.Opening the database" << std::endl;

    return true;
}

bool DataManager::init_pragmas()
{
    std::cout <<"2-1 Configuring runtime parameters" << std::endl;

    m_last_error.clear();

    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    if (!db.isValid() || !db.isOpen())
    {
        m_last_error = "Database is not open.";
        return false;
    }

    QSqlQuery q(db);

    auto exec_sql = [&](const QString& sql) -> bool
        {
            if (!q.exec(sql))
            {
                m_last_error = q.lastError().text() + " | SQL: " + sql;
                return false;
            }
            return true;
        };

    if (!exec_sql("PRAGMA foreign_keys = ON;"))
        return false;

    if (!q.exec("PRAGMA journal_mode = WAL;"))
    {
        if (!q.exec("PRAGMA journal_mode = DELETE;"))
        {
            m_last_error = q.lastError().text() + " | SQL: PRAGMA journal_mode";
            return false;
        }
    }

    if (!exec_sql("PRAGMA synchronous = NORMAL;"))
        return false;

    if (!exec_sql("PRAGMA temp_store = MEMORY;"))
        return false;

    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");

    std::cout << "2-2 Configuring runtime parameters" << std::endl;

    return true;
}

bool DataManager::init_schema()
{
    std::cout <<"3-1 Building the table structure"<< std::endl;
    m_last_error.clear();

    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    if (!db.isValid() || !db.isOpen())
    {
        m_last_error = "Database is not open.";
        return false;
    }

    QSqlQuery q(db);

    auto exec_sql = [&](const QString& sql) -> bool
        {
            if (!q.exec(sql))
            {
                m_last_error = q.lastError().text() + " | SQL: " + sql;
                return false;
            }
            return true;
        };

    // campus
    if (!exec_sql(
        "create table if not exists campus ("
        "    campus_id integer primary key autoincrement,"
        "    name text not null unique"
        ");"))
        return false;

    // souvenir
    if (!exec_sql(
        "create table if not exists souvenir ("
        "    souvenir_id integer primary key autoincrement,"
        "    campus_id integer not null,"
        "    name text not null,"
        "    price real not null check(price >= 0),"
        "    foreign key(campus_id) references campus(campus_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    unique(campus_id, name)"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_souvenir_campus_id on souvenir(campus_id);"))
        return false;

    // distance (store undirected edge once: a_campus_id < b_campus_id)
    if (!exec_sql(
        "create table if not exists distance ("
        "    a_campus_id integer not null,"
        "    b_campus_id integer not null,"
        "    miles real not null check(miles >= 0),"
        "    primary key(a_campus_id, b_campus_id),"
        "    foreign key(a_campus_id) references campus(campus_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(b_campus_id) references campus(campus_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    check(a_campus_id < b_campus_id)"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_distance_a on distance(a_campus_id);"))
        return false;

    if (!exec_sql("create index if not exists idx_distance_b on distance(b_campus_id);"))
        return false;

    // trip
    if (!exec_sql(
        "create table if not exists trip ("
        "    trip_id integer primary key autoincrement,"
        "    start_campus_id integer not null,"
        "    created_at text not null default (datetime('now')),"
        "    total_miles real not null default 0 check(total_miles >= 0),"
        "    foreign key(start_campus_id) references campus(campus_id)"
        "        on delete restrict"
        "        on update cascade"
        ");"))
        return false;

    // purchase
    if (!exec_sql(
        "create table if not exists purchase ("
        "    trip_id integer not null,"
        "    campus_id integer not null,"
        "    souvenir_id integer not null,"
        "    quantity integer not null check(quantity > 0),"
        "    unit_price real not null check(unit_price >= 0),"
        "    primary key(trip_id, souvenir_id),"
        "    foreign key(trip_id) references trip(trip_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(campus_id) references campus(campus_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(souvenir_id) references souvenir(souvenir_id)"
        "        on delete cascade"
        "        on update cascade"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_purchase_trip_id on purchase(trip_id);"))
        return false;

    if (!exec_sql("create index if not exists idx_purchase_campus_id on purchase(campus_id);"))
        return false;

    std::cout << "3-2 Building the table structure" << std::endl;

    return true;
}

bool DataManager::seed_if_empty()
{
    std::cout <<"Loading data"<<std::endl;

    return true;
}

bool DataManager::is_open() const
{
    if (!QSqlDatabase::contains(m_conn_name))
        return false;

    const QSqlDatabase db = QSqlDatabase::database(m_conn_name, false);

    return db.isValid() && db.isOpen();
}
