#ifndef USRCONTROL_H
#define USRCONTROL_H
#include "Usr.h"
#include <QVector>
#include <QMap>
#include "DBHelper.h"
#include <QSqlQuery>
#include "package.h"

class UsrControl
{
private:
    UsrControl();
    UsrControl(const UsrControl & usr) = delete;
    UsrControl& operator = (const UsrControl & usr) = delete;
    static UsrControl* instance;

    QMap<int, Usr> Usrs;
public:

    static UsrControl* getInstance();

    QMap<int, Usr> getUsrs(){ return this->Usrs; }

    //使用ID找到对应用户信息-创建房间或加入房间
    Usr& findUsrById(int key);

    //向容器中添加用户信息-服务器建立连接时或新注册时
    void insertUsrIntoUsrs(int key, Usr& usrFromDB);

    //从容器中删除用户信息-用户下线
    void deleteUsrFromUsrs(int key);

    //向数据库内添加用户信息-新用户注册
    bool insertUsr(const Usr& usr);
    //删除学员信息
    bool deleteUsr(int id);
    //修改学员信息\充值
    bool changeUsr(int id, int reMoney);
    //充值
    bool recharge(int reMoney);
    //消费
    bool consume();
    //修改密码
    bool changePw(int id, const QString & pw);
    //按照姓氏查找
    QVector<Usr> findUsrName(const QString & name);
    //按照学号查找
    Usr findUsrId(int id);
    //判断账号重复
    bool judgDoubleName(const QString & name);
    //按照账号登录
    Usr logByName(const QString & name, const QString & pw);
    //更改上线状态
    bool onlineState(const QString & name);
    //更改下线状态
    bool logOutState(const QString & name);
    //按照名字查找ID
    int getIdByName(const QString & name);
    //找回密码
    Usr & findPw(pack_t & pack);

    void getGift(pack_t & pack);

    int disconnect(QTcpSocket * socket);
};

#endif // USRCONTROL_H
