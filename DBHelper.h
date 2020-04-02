#ifndef DBHELPER_H
#define DBHELPER_H
#include <QSqlDatabase>

class DBHelper
{
private:
    DBHelper();

    DBHelper(const DBHelper&) = delete;

    DBHelper& operator = (const DBHelper&) = delete;

    static DBHelper *instance;

    QSqlDatabase db;

public:
    static DBHelper * getInstance();

    bool createConnect();
    void removeConnect();
};


#endif // DBHELPER_H
