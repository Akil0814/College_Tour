#include "data_manager.h"

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

bool DataManager::is_open() const
{
    if (!QSqlDatabase::contains(m_conn_name))
        return false;

    const QSqlDatabase db = QSqlDatabase::database(m_conn_name, false);

    return db.isValid() && db.isOpen();
}

QString DataManager::last_error() const 
{
    return m_last_error;
}

QVector<college> DataManager::get_all_colleges() const
{
    QVector<college> out;

    QSqlDatabase db = get_db_or_set_error();

    if (!db.isValid())
        return out;

    QSqlQuery q(db);
    const QString sql =
        "select college_id, name "
        "from college " 
        "order by college_id;";

    if (!prepare_and_exec(q, sql))
        return out;


    while (q.next())
    {
        college c;
        c.college_id = q.value(0).toInt();
        c.name = q.value(1).toString();
        out.push_back(c);
    }

    return out;

}

QVector<souvenir> DataManager::get_all_souvenirs() const
{
    QVector<souvenir> out;

    QSqlDatabase db = get_db_or_set_error();
    if (!db.isValid())
        return out;

    QSqlQuery q(db);

    const QString sql =
        "select souvenir_id, college_id, name, price "
        "from souvenir "
        "order by college_id, souvenir_id;";

    if (!prepare_and_exec(q, sql))
        return out;

    while (q.next())
    {
        souvenir s;
        s.souvenir_id = q.value(0).toInt();
        s.owner_college_id = q.value(1).toInt();
        s.name = q.value(2).toString();
        s.price = q.value(3).toDouble();
        out.push_back(s);
    }

    return out;
}


//-----------------private-----------------//

static QStringList parse_csv_line(const QString& line)
{
    QStringList out;
    QString cur;
    bool in_quotes = false;

    for (int i = 0; i < line.size(); ++i)
    {
        const QChar c = line[i];

        if (in_quotes)
        {
            if (c == '"')
            {
                // escaped quote
                if (i + 1 < line.size() && line[i + 1] == '"')
                {
                    cur += '"';
                    ++i;
                }
                else
                    in_quotes = false;
            }
            else
                cur += c;
        }
        else
        {
            if (c == '"')
                in_quotes = true;
            else if (c == ',')
            {
                out.push_back(cur);
                cur.clear();
            }
            else
                cur += c;
        }
    }

    out.push_back(cur);
    return out;
}

static QString norm_name(QString s)
{
    return s.trimmed();
}

static bool read_csv_file_lines(const QString& path, QStringList& lines, QString& err)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        err = "Cannot open CSV: " + path;
        return false;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.isEmpty())
            lines.push_back(line);
    }

    return true;
}

static bool parse_money_to_double(QString s, double& out_price)
{
    s = s.trimmed();
    if (s.isEmpty())
        return false;

    // remove $ and commas and spaces
    s.remove('$');
    s.remove(',');
    s = s.trimmed();

    bool ok = false;
    double v = s.toDouble(&ok);
    if (!ok)
        return false;

    out_price = v;
    return true;
}

//-----------------private-----------------//

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

    // college
    if (!exec_sql(
        "create table if not exists college ("
        "    college_id integer primary key autoincrement,"
        "    name text not null unique"
        ");"))
        return false;

    // souvenir
    if (!exec_sql(
        "create table if not exists souvenir ("
        "    souvenir_id integer primary key autoincrement,"
        "    college_id integer not null,"
        "    name text not null,"
        "    price real not null check(price >= 0),"
        "    foreign key(college_id) references college(college_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    unique(college_id, name)"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_souvenir_college_id on souvenir(college_id);"))
        return false;

    // distance (store undirected edge once: a_college_id < b_college_id)
    if (!exec_sql(
        "create table if not exists distance ("
        "    a_college_id integer not null,"
        "    b_college_id integer not null,"
        "    miles real not null check(miles >= 0),"
        "    primary key(a_college_id, b_college_id),"
        "    foreign key(a_college_id) references college(college_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(b_college_id) references college(college_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    check(a_college_id < b_college_id)"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_distance_a on distance(a_college_id);"))
        return false;

    if (!exec_sql("create index if not exists idx_distance_b on distance(b_college_id);"))
        return false;

    // trip
    if (!exec_sql(
        "create table if not exists trip ("
        "    trip_id integer primary key autoincrement,"
        "    start_college_id integer not null,"
        "    created_at text not null default (datetime('now')),"
        "    total_miles real not null default 0 check(total_miles >= 0),"
        "    foreign key(start_college_id) references college(college_id)"
        "        on delete restrict"
        "        on update cascade"
        ");"))
        return false;

    // purchase
    if (!exec_sql(
        "create table if not exists purchase ("
        "    trip_id integer not null,"
        "    college_id integer not null,"
        "    souvenir_id integer not null,"
        "    quantity integer not null check(quantity > 0),"
        "    unit_price real not null check(unit_price >= 0),"
        "    primary key(trip_id, souvenir_id),"
        "    foreign key(trip_id) references trip(trip_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(college_id) references college(college_id)"
        "        on delete cascade"
        "        on update cascade,"
        "    foreign key(souvenir_id) references souvenir(souvenir_id)"
        "        on delete cascade"
        "        on update cascade"
        ");"))
        return false;

    if (!exec_sql("create index if not exists idx_purchase_trip_id on purchase(trip_id);"))
        return false;

    if (!exec_sql("create index if not exists idx_purchase_college_id on purchase(college_id);"))
        return false;

    std::cout << "3-2 Building the table structure" << std::endl;

    return true;
}

bool DataManager::seed_if_empty()
{
    std::cout << "4-1 Loading data" << std::endl;
    m_last_error.clear();

    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    if (!db.isValid() || !db.isOpen())
    {
        m_last_error = "Database is not open.";
        return false;
    }

    // already seeded?
    {
        QSqlQuery q(db);
        if (!q.exec("select count(*) from college;"))
        {
            m_last_error = q.lastError().text();
            return false;
        }
        if (q.next() && q.value(0).toInt() > 0)
        {
            std::cout << "4-2 Loading data" << std::endl;
            return true;
        }
    }

    // data dir comes from db path: .../data/college_tour.db
    const QString data_dir = QFileInfo(m_db_path).dir().absolutePath();
    const QString souvenirs_csv = QDir(data_dir).filePath("college_souvenirs.csv");
    const QString distances_csv = QDir(data_dir).filePath("college_campus_distances.csv");

    QStringList souvenir_lines;
    QStringList distance_lines;
    QString file_err;

    if (!read_csv_file_lines(souvenirs_csv, souvenir_lines, file_err))
    {
        m_last_error = file_err;
        return false;
    }

    if (!read_csv_file_lines(distances_csv, distance_lines, file_err))
    {
        m_last_error = file_err;
        return false;
    }

    // 1) collect college names from both csv
    QSet<QString> college_set;

    // souvenirs: "College" appears at block headers, sometimes other rows have blank college
    for (int i = 0; i < souvenir_lines.size(); ++i)
    {
        const QStringList cols = parse_csv_line(souvenir_lines[i]);

        if (i == 0)
            continue; // header row

        if (cols.size() >= 1)
        {
            const QString college = norm_name(cols[0]);
            if (!college.isEmpty())
                college_set.insert(college);
        }
    }

    // distances: header: Starting College, Ending College, Distance Between
    for (int i = 0; i < distance_lines.size(); ++i)
    {
        const QStringList cols = parse_csv_line(distance_lines[i]);
        if (i == 0)
            continue;

        if (cols.size() >= 2)
        {
            const QString a = norm_name(cols[0]);
            const QString b = norm_name(cols[1]);
            if (!a.isEmpty()) college_set.insert(a);
            if (!b.isEmpty()) college_set.insert(b);
        }
    }

    // 2) transaction
    if (!db.transaction())
    {
        m_last_error = db.lastError().text();
        return false;
    }

    auto rollback_with = [&](const QString& err) -> bool
        {
            m_last_error = err;
            db.rollback();
            return false;
        };

    // 3) insert collegees
    {
        QSqlQuery ins(db);
        if (!ins.prepare("insert or ignore into college(name) values(?);"))
            return rollback_with(ins.lastError().text());

        for (const QString& name : college_set)
        {
            ins.bindValue(0, name);
            if (!ins.exec())
                return rollback_with(ins.lastError().text() + " | college: " + name);
        }
    }

    // 4) build name -> id map
    QHash<QString, int> college_id_by_name;
    {
        QSqlQuery q(db);
        if (!q.exec("select college_id, name from college;"))
            return rollback_with(q.lastError().text());

        while (q.next())
        {
            const int id = q.value(0).toInt();
            const QString name = q.value(1).toString();
            college_id_by_name.insert(name, id);
        }
    }

    // 5) insert souvenirs (block format)
    {
        QSqlQuery up(db);
        const QString sql =
            "insert into souvenir(college_id, name, price) values(?, ?, ?) "
            "on conflict(college_id, name) do update set price=excluded.price;";
        if (!up.prepare(sql))
            return rollback_with(up.lastError().text());

        QString current_college;

        for (int i = 1; i < souvenir_lines.size(); ++i) // skip header
        {
            const QStringList cols = parse_csv_line(souvenir_lines[i]);
            if (cols.size() < 3)
                continue;

            const QString college_cell = norm_name(cols[0]);
            const QString item = norm_name(cols[1]);
            const QString cost = cols[2];

            if (!college_cell.isEmpty())
                current_college = college_cell;

            if (current_college.isEmpty())
                continue;

            if (item.isEmpty())
                continue;

            double price = 0.0;
            if (!parse_money_to_double(cost, price))
                continue;

            if (!college_id_by_name.contains(current_college))
                return rollback_with("Unknown college in souvenirs CSV: " + current_college);

            const int college_id = college_id_by_name.value(current_college);

            up.bindValue(0, college_id);
            up.bindValue(1, item);
            up.bindValue(2, price);

            if (!up.exec())
                return rollback_with(up.lastError().text() + " | souvenir: " + current_college + " / " + item);
        }
    }

    // 6) insert distances (undirected: store (min_id, max_id))
    {
        QSqlQuery up(db);
        const QString sql =
            "insert into distance(a_college_id, b_college_id, miles) values(?, ?, ?) "
            "on conflict(a_college_id, b_college_id) do update set miles=excluded.miles;";
        if (!up.prepare(sql))
            return rollback_with(up.lastError().text());

        for (int i = 1; i < distance_lines.size(); ++i) // skip header
        {
            const QStringList cols = parse_csv_line(distance_lines[i]);
            if (cols.size() < 3)
                continue;

            const QString a_name = norm_name(cols[0]);
            const QString b_name = norm_name(cols[1]);
            const QString miles_s = cols[2].trimmed();

            if (a_name.isEmpty() || b_name.isEmpty())
                continue;

            if (!college_id_by_name.contains(a_name))
                return rollback_with("Unknown college in distances CSV: " + a_name);

            if (!college_id_by_name.contains(b_name))
                return rollback_with("Unknown college in distances CSV: " + b_name);

            bool ok = false;
            double miles = miles_s.toDouble(&ok);
            if (!ok)
                continue;

            int a_id = college_id_by_name.value(a_name);
            int b_id = college_id_by_name.value(b_name);

            if (a_id == b_id)
                continue;

            if (a_id > b_id)
                std::swap(a_id, b_id);

            up.bindValue(0, a_id);
            up.bindValue(1, b_id);
            up.bindValue(2, miles);

            if (!up.exec())
                return rollback_with(up.lastError().text() + " | distance: " + a_name + " - " + b_name);
        }
    }

    if (!db.commit())
        return rollback_with(db.lastError().text());

    std::cout << "4-x Loading data" << std::endl;

    return true;
}

QSqlDatabase DataManager::get_db_or_set_error() const
{
    if (!QSqlDatabase::contains(m_conn_name))
    {
        m_last_error = "database connection not found";
        return {};
    }

    QSqlDatabase db = QSqlDatabase::database(m_conn_name, false);
    if (!db.isValid() || !db.isOpen())
    {
        m_last_error = "database is not open";
        return {};
    }

    return db;
}

bool DataManager::prepare_and_exec(QSqlQuery& q, const QString& sql) const
{
    if (!q.prepare(sql))
    {
        m_last_error = "prepare failed: " + q.lastError().text() + " | sql=" + sql;
        return false;
    }

    if (!q.exec())
    {
        m_last_error = "exec failed: " + q.lastError().text() + " | sql=" + sql;
        return false;
    }

    return true;
}
