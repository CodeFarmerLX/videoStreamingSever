#include "Usr.h"

Usr::Usr()
{

}

Usr::Usr(int id, const QString& name, const QString & pw, int tmoney, int money, const QString& phone, int point)
    :id(id),name(name),pw(pw),tmoney(tmoney),money(money),phone(phone),point(point)
{

}

Usr::Usr(const QString & name, const QString& pw)
    :name(name),pw(pw)
{

}

Usr::Usr(int id, const QString & name, int tmoney, int point)
    :id(id),name(name),tmoney(tmoney),point(point)
{

}

Usr::Usr(const QString &name, const QString & pw, const QString& phone)
    :name(name),pw(pw),phone(phone)
{

}

Usr::Usr(int id, const QString &name, const QString& phone)
    :id(id), name(name),phone(phone)
{

}
