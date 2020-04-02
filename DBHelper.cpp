#include "DBHelper.h"
#include <QDebug>

DBHelper* DBHelper::instance = 0;

DBHelper::DBHelper()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DBHelper* DBHelper::getInstance()
{
    if(instance == 0)
        instance = new DBHelper();
    return instance;
}

bool DBHelper::createConnect()
{
    db.setDatabaseName("usrinfo.db");
    if(!db.open()){
        qDebug() << "数据库连接失败";
        return false;
    }
    return true;
}

void DBHelper::removeConnect()
{
    db.close();
    db.removeDatabase("usrinfo.db");
}
