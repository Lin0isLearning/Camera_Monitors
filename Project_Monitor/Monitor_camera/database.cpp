#include "database.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

Database* db = nullptr;

Database::Database(const QString &database_name) : db_name(database_name) {}

Database::~Database() {
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::initialize() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_name);

    if (!db.open()) {
        qDebug() << "Cannot open database: " << db.lastError().text();
        return false;
    }

    QString create_table_sql = "CREATE TABLE IF NOT EXISTS users ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "username TEXT NOT NULL UNIQUE, "
                               "password TEXT NOT NULL);";
    return executeQuery(create_table_sql);
}

bool Database::registerUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "Register user failed: " << query.lastError().text();
        return false;
    }

    return true;
}

bool Database::authenticateUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT 1 FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.exec();

    return query.next();
}

bool Database::executeQuery(const QString &query) {
    QSqlQuery q;
    if (!q.exec(query)) {
        qDebug() << "SQL error: " << q.lastError().text();
        return false;
    }
    return true;
}
