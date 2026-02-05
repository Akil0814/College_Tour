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
    QDir exeDir(QCoreApplication::applicationDirPath());

    // Go up a few levels to reach the project root (adjust if your build tree differs)
    exeDir.cdUp(); // Desktop_...-Debug
    exeDir.cdUp(); // build
    exeDir.cdUp(); // College_Tour (project root)  <-- adjust if needed

    if (!exeDir.cd("data")) {
        m_last_error = "Cannot find project data folder from: " + exeDir.absolutePath();
        return false;
    }

    m_db_path = exeDir.filePath("college_tour.db");

    QSqlDatabase db = QSqlDatabase::contains(m_conn_name)
        ? QSqlDatabase::database(m_conn_name)
        : QSqlDatabase::addDatabase("QSQLITE", m_conn_name);

    db.setDatabaseName(m_db_path);

    if (!db.open()) {
        m_last_error = db.lastError().text();
        return false;
    }
    return true;
}

bool DataManager::init_pragmas()
{
    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    QSqlQuery q(db);

    if (!q.exec("PRAGMA foreign_keys = ON;"))
    {
        m_last_error = q.lastError().text();
        return false;
    }

    q.exec("PRAGMA journal_mode = WAL;");

    return true;
}

bool DataManager::init_schema()
{
    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    QSqlQuery q(db);

    if (!db.transaction()) {
        m_last_error = db.lastError().text();
        return false;
    }

    // 1) campus
    if (!exec_sql(q, m_last_error, R"sql(
        CREATE TABLE IF NOT EXISTS campus(
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        );
    )sql")) {
        db.rollback(); return false;
    }

    // 2) distance (store only one row for an undirected pair: (a,b) where a < b)
    if (!exec_sql(q, m_last_error, R"sql(
        CREATE TABLE IF NOT EXISTS distance(
            a     INTEGER NOT NULL,
            b     INTEGER NOT NULL,
            miles REAL    NOT NULL CHECK(miles >= 0),
            PRIMARY KEY(a, b),
            FOREIGN KEY(a) REFERENCES campus(id) ON DELETE CASCADE,
            FOREIGN KEY(b) REFERENCES campus(id) ON DELETE CASCADE,
            CHECK(a < b)
        );
    )sql")) {
        db.rollback(); return false;
    }

    // 3) souvenir
    if (!exec_sql(q, m_last_error, R"sql(
        CREATE TABLE IF NOT EXISTS souvenir(
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            campus_id  INTEGER NOT NULL,
            name       TEXT    NOT NULL,
            price_cents INTEGER NOT NULL CHECK(price_cents >= 0),
            UNIQUE(campus_id, name),
            FOREIGN KEY(campus_id) REFERENCES campus(id) ON DELETE CASCADE
        );
    )sql")) {
        db.rollback(); return false;
    }

    // 4) purchase (order)
    if (!exec_sql(q, m_last_error, R"sql(
        CREATE TABLE IF NOT EXISTS purchase(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            created_at TEXT NOT NULL DEFAULT (datetime('now'))
        );
    )sql")) {
        db.rollback(); return false;
    }

    // 5) purchase_item (order lines)
    if (!exec_sql(q, m_last_error, R"sql(
        CREATE TABLE IF NOT EXISTS purchase_item(
            purchase_id INTEGER NOT NULL,
            souvenir_id INTEGER NOT NULL,
            qty INTEGER NOT NULL CHECK(qty > 0),
            price_cents INTEGER NOT NULL CHECK(price_cents >= 0),
            PRIMARY KEY(purchase_id, souvenir_id),
            FOREIGN KEY(purchase_id) REFERENCES purchase(id) ON DELETE CASCADE,
            FOREIGN KEY(souvenir_id) REFERENCES souvenir(id) ON DELETE RESTRICT
        );
    )sql")) {
        db.rollback(); return false;
    }

    // Helpful indexes (optional, but cheap)
    exec_sql(q, m_last_error, "CREATE INDEX IF NOT EXISTS idx_souvenir_campus ON souvenir(campus_id);");
    exec_sql(q, m_last_error, "CREATE INDEX IF NOT EXISTS idx_purchase_item_pid ON purchase_item(purchase_id);");

    if (!db.commit()) {
        m_last_error = db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

bool DataManager::seed_if_empty()
{
    QSqlDatabase db = QSqlDatabase::database(m_conn_name);
    QSqlQuery q(db);

    // 1) If campus already has data, assume seeding was done before.
    if (!q.exec("SELECT 1 FROM campus LIMIT 1;")) {
        m_last_error = q.lastError().text();
        return false;
    }
    if (q.next()) {
        return true; // not empty -> no seeding
    }

    // 2) Seed within a transaction (all or nothing).
    if (!db.transaction()) {
        m_last_error = db.lastError().text();
        return false;
    }

    auto fail = [&](const QSqlQuery& qq) {
        m_last_error = qq.lastError().text();
        db.rollback();
        return false;
        };

    // Insert campuses
    QSqlQuery insCampus(db);
    if (!insCampus.prepare("INSERT INTO campus(name) VALUES(?);")) return fail(insCampus);

    const QStringList campuses = { "UCI", "UCLA", "UCSD" };
    for (const QString& name : campuses) {
        insCampus.addBindValue(name);
        if (!insCampus.exec()) return fail(insCampus);
    }

    // Helper: get campus id by name
    auto getCampusId = [&](const QString& name) -> int {
        QSqlQuery qq(db);
        qq.prepare("SELECT id FROM campus WHERE name=?;");
        qq.addBindValue(name);
        if (!qq.exec() || !qq.next()) return -1;
        return qq.value(0).toInt();
        };

    const int uci = getCampusId("UCI");
    const int ucla = getCampusId("UCLA");
    const int ucsd = getCampusId("UCSD");
    if (uci < 0 || ucla < 0 || ucsd < 0) {
        m_last_error = "seed_if_empty: failed to fetch campus ids";
        db.rollback();
        return false;
    }

    // Insert distances (store only (a,b) where a < b)
    auto upsertDistance = [&](int x, int y, double miles) -> bool {
        const int a = std::min(x, y);
        const int b = std::max(x, y);
        QSqlQuery qq(db);
        qq.prepare("INSERT OR REPLACE INTO distance(a,b,miles) VALUES(?,?,?);");
        qq.addBindValue(a);
        qq.addBindValue(b);
        qq.addBindValue(miles);
        if (!qq.exec()) return fail(qq);
        return true;
        };

    if (!upsertDistance(uci, ucla, 45.0))  return false;
    if (!upsertDistance(uci, ucsd, 80.0))  return false;
    if (!upsertDistance(ucla, ucsd, 120.0)) return false;

    // Insert souvenirs
    auto addSouvenir = [&](int campusId, const QString& item, int priceCents) -> bool {
        QSqlQuery qq(db);
        qq.prepare("INSERT INTO souvenir(campus_id, name, price_cents) VALUES(?,?,?);");
        qq.addBindValue(campusId);
        qq.addBindValue(item);
        qq.addBindValue(priceCents);
        if (!qq.exec()) return fail(qq);
        return true;
        };

    if (!addSouvenir(uci, "Sticker", 299))  return false;
    if (!addSouvenir(uci, "T-Shirt", 1999)) return false;
    if (!addSouvenir(ucla, "Mug", 1299)) return false;
    if (!addSouvenir(ucsd, "Hoodie", 4999)) return false;

    if (!db.commit()) {
        m_last_error = db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}
