#ifndef USR_H
#define USR_H
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>

#if 0
create table usr(
        uid integer primary key autoincrement,
        uname text not null,
        upw text default 123456,
        utmoney integer default 0,
        umoney integer default 0,
        uphone text not null,
        upoint integer default 0,
        ustate integer default 0
        );

create table usrIdInformation(
        id integer primary key autoincrement,
        idNumber text not null,
        name text not null
        );

create table usrState(
        id integer not null,
        state integer not null
        );





#endif


class Usr
{
private:
    int id;
    QString name;
    QString pw;
    int tmoney;
    int money;
    QString phone;
    int point;
    QHostAddress ipAddress;
    QTcpSocket* usrSocket;

public:
    Usr();
    Usr(int id, const QString& name, const QString & pw, int tmoney, int money, const QString& phone, int point = 0);//展示全部信息
    Usr(const QString & name, const QString& pw);//登录
    Usr(int id, const QString & name, int tmoney, int point);//展示消费金额构造函数
    Usr(const QString & name, const QString & pw, const QString& phone);//注册用构造函数
    Usr(int id, const QString & name, const QString& phone);//找回密码构造函数

    void setId(int id) { this->id = id; }
    void setName(QString name) { this->name = name; }
    void setPw(QString pw) { this->pw = pw; }
    void setTMoney(int tmoney) { this->tmoney = tmoney; }
    void setMoney(int money) { this->money = money; }
    void setPhone(QString phone){ this->phone = phone; }
    void setPoint(int point) { this->point = point; }
    void setIpAddress(QHostAddress ipAddress){ this->ipAddress = ipAddress; }
    void setUsrSocket(QTcpSocket * usrsocket) { this->usrSocket = usrsocket; }

    int getId()const { return this->id; }
    const QString & getName()const{ return this->name; }
    const QString & getPw()const{ return this->pw; }
    int getTMoney()const{ return this->tmoney; }
    int getMoney()const{ return this->money; }
    const QString& getPhone()const{ return this->phone; }
    int getPoint()const { return this->point; }
    QHostAddress getIpAddress() const { return this->ipAddress; }
    QTcpSocket * getUsrSocket()const{ return this->usrSocket; }


};

#endif // USR_H
