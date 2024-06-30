#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

class Database {
public:
    Database(const QString &database_name);
    ~Database();

    bool initialize();
    bool registerUser(const QString &username, const QString &password);
    bool authenticateUser(const QString &username, const QString &password);

private:
    QSqlDatabase db;
    QString db_name;

    bool executeQuery(const QString &query);
};

extern Database* db;

#endif // DATABASE_H
