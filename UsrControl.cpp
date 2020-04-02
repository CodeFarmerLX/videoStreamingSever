#include "UsrControl.h"
#include <QVariant>
#include <QMessageBox>
#include <QDebug>
#include "package.h"
#include <QTcpSocket>

UsrControl* UsrControl::instance = 0;

UsrControl::UsrControl()
{

}

UsrControl* UsrControl::getInstance()
{
//    qDebug() << "用户控制单例 ： " << instance;
    if(0 == instance)
    {
        instance = new UsrControl();
    }
    return instance;
}

//使用ID找到对应用户信息
Usr& UsrControl::findUsrById(int key)
{
    QMap<int, Usr>::iterator it = this->Usrs.find(key);
//    qDebug() << "按照ID寻找用户的名字" << (*it).getName();

    if(Usrs.end() == it){
//        qDebug() << "没用按照ID找到对应用户信息";
    }
    else
    {
//        qDebug() << "找到对应用户）";
    }
    return *it;
}

//向容器中添加用户信息
void UsrControl::insertUsrIntoUsrs(int key, Usr& usrFromDB)
{
    this->Usrs.insert(key, usrFromDB);
    QMap<int, Usr>::iterator it = Usrs.find(key);
    if(Usrs.end() == it)
    {
        qDebug() << "向容器中添加用户信息失败（登录成功时）";
    }
    else
    {
        qDebug() << "向容器中添加用户信息成功（登录成功时）";
    }
}


//从容器中删除用户信息
void UsrControl::deleteUsrFromUsrs(int key)
{
    QMap<int, Usr>::iterator it = this->Usrs.find(key);

    if(Usrs.end() == it){
        qDebug() << "从容器中删除用户信息失败（下线）";
    }
    else
    {
        Usrs.erase(it);
        qDebug() << "从容器中删除用户信息成功（下线）";
    }
}



//更改上线状态
bool UsrControl::onlineState(const QString & name)
{
    DBHelper * open =  DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("select * from usr where uname = :name");
    query.bindValue(":name", name);
    if(query.exec()){
        while (query.next()) {
            int id = query.value(7).toInt();
            if(id == 1){
                qDebug() << "用户在线";
                return false;
            }
        }
    }
    else{
        qDebug() << "查找用户语句未执行";
    }

    query.prepare("update usr set ustate = 1 where uname = :name");
    query.bindValue(":name", name);
    bool ret = query.exec();

    if(!ret){
        qDebug() << "更改上线语句失败";
    }
    open->removeConnect();
    return ret;
}

//更改下线状态
bool UsrControl::logOutState(const QString & name)
{
    DBHelper * open =  DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("update usr set ustate = 0 where uname = :name");
    query.bindValue(":name", name);
    bool ret = query.exec();

    if(!ret){
        qDebug() << "更改下线语句失败";
    }
    open->removeConnect();
    return ret;
}

//判断名字是否重复
bool UsrControl::judgDoubleName(const QString & name)
{
    DBHelper * open = DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("select * from usr where uname = :name");

    query.bindValue(":name", name);

    query.exec();

    int n = 0;
    while(query.next()){
        n++;
    }
    if(0 == n)
        return true;
    else
        return false;
}
//添加用户信息\注册
bool UsrControl::insertUsr(const Usr& usr)
{
//    qDebug() << "insert failed 1";
    DBHelper * open = DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    if(!judgDoubleName(usr.getName())){
        qDebug() << "名字重复";
        return false;
    }

    query.prepare("insert into usr (uname,upw,uphone) values(:uname, :upw, :uphone)");

    query.bindValue(":uname",usr.getName());
    query.bindValue(":upw", usr.getPw());
    query.bindValue(":uphone",usr.getPhone());

    int ret = query.exec();


    if(!ret){
        qDebug() << "插入失败";
        open->removeConnect();
        return false;
    }
    else{
        qDebug() << "插入用户信息列表成功";
        open->removeConnect();
        return true;
    }
}
//删除数据库用户信息
bool UsrControl::deleteUsr(int id)
{
    DBHelper * open = DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("delete from usr where uid = :id");

    query.bindValue(":id", id);

    int ret = query.exec();
    open->removeConnect();

    return ret;
}
//修改用户信息\充值
bool UsrControl::changeUsr(int id, int reMoney)
{
    DBHelper * open = DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("select * from usr where uid = :id");

    query.bindValue(":id", id);
    int money = 0;
    query.exec();
    while(query.next()){
        money  = query.value(4).toInt();
    }

    int tmoney = reMoney + money;


    query.prepare("update usr set utmoney = :utmoney where uid = :id");

    query.bindValue(":id", id);
    query.bindValue(":utmoney", tmoney);

    int ret = query.exec();
    open->removeConnect();

    return ret;
}
////充值
//bool UsrControl::recharge(int reMoney){ return true; }
////消费
//bool UsrControl::consume(){ return true; }
//修改密码
bool UsrControl::changePw(int id, const QString& pw)
{
    DBHelper * open = DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    query.prepare("update usr set upw = :upw where uid = :id");

    query.bindValue(":id", id);
    query.bindValue(":upw", pw);

    int ret = query.exec();
    open->removeConnect();

    return ret;
}

//按照姓氏查找
QVector<Usr> UsrControl::findUsrName(const QString & name)
{
    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();


    QVector<Usr> usrs;
    //对表单的操作
    QSqlQuery query;
    //先准备
    query.prepare("select *from usr where uname like :name");
    //绑定
    Usr usr(0," ",0,0);
    if(name == " "){
        instance->removeConnect();
        return usrs;
    }
    query.bindValue(":name", name + "%");
    //执行
    if(query.exec()){

       while(query.next()){
            int id = query.value(0).toInt();
            QString sname = query.value(1).toString();
            QString upw = query.value(2).toString();
            int utmoney = query.value(3).toInt();
            int umoney = query.value(4).toInt();

            usr.setId(id);
            usr.setName(sname);
            usr.setPw(upw);
            usr.setTMoney(utmoney);
            usr.setMoney(umoney);

            usrs.push_back(usr);
       }
    }
    else{
        qDebug() << "查找Name失败";
    }
    instance->removeConnect();
    return usrs;
}
//

Usr UsrControl::findUsrId(int id)
{
    //打开数据库
    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();

    //对表单的操作
    QSqlQuery query;
    //先准备
    query.prepare("select *from usr where uid = :id");
    //绑定
    Usr usr(0," ","0",0,0,0);
    if(id == 0){
        instance->removeConnect();
        return usr;
    }

    query.bindValue(":id", id);
    qDebug() << "ID";
    //执行
    if(query.exec()){

       while(query.next()){
           int sid = query.value(0).toInt();
           QString sname = query.value(1).toString();
           QString upw = query.value(2).toString();
           int utmoney = query.value(3).toInt();
           int umoney = query.value(4).toInt();

           usr.setId(sid);
           usr.setName(sname);
           usr.setPw(upw);
           usr.setTMoney(utmoney);
           usr.setMoney(umoney);
       }
    }
    else{
        qDebug() << "查找ID失败";
    }
    instance->removeConnect();
    return usr;
}

//按照账号登录
Usr UsrControl::logByName(const QString & name, const QString & pw)
{
    qDebug() << "按照账号登录";
    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();

    //对表单的操作
    QSqlQuery query;
    //先准备
    query.prepare("select *from usr where uname = :name");
    //绑定
    Usr usr(0,"Don not find","0",0,0,0);
//    if(name == " "){
//        instance->removeConnect();
//        return usr;
//    }
    query.bindValue(":name", name);
//    qDebug() << "1";
    //执行
    if(query.exec()){
        qDebug() << "登录账号寻找成功";
       while(query.next()){
            int id = query.value(0).toInt();
            QString sname = query.value(1).toString();
            QString upw = query.value(2).toString();
            int utmoney = query.value(3).toInt();
            int umoney = query.value(4).toInt();
            int upoint = query.value(6).toInt();

            if(upw != pw){
                usr.setName("Pw is false");
                qDebug() << "密码不正确";
                break;
            }

            usr.setId(id);
            usr.setName(sname);
            usr.setPw(upw);
            usr.setTMoney(utmoney);
            usr.setMoney(umoney);
            usr.setPoint(upoint);

            qDebug() << "登录账号寻找成功：姓名";
       }
    }
    else{
        usr.setName("Don not find");
        qDebug() << "查找Name失败1";
    }
    instance->removeConnect();
    return usr;
}

//通过名字获得ID
int UsrControl::getIdByName(const QString & name)
{
    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();

    //对表单的操作
    QSqlQuery query;
    //先准备
    query.prepare("select *from usr where uname = :name");

    query.bindValue(":name",name);

    int id = 0;

    if(query.exec()){
        while(query.next()){
            id = query.value(0).toInt();
        }
    }
    else{
        qDebug() << "查找Name返回ID失败";
    }
    return id;
}

//找回密码
Usr & UsrControl::findPw(pack_t & pack)
{
    QString name = QString::fromLocal8Bit(pack.name);
    QString phone = QString::fromLocal8Bit(pack.phone);

    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();

    Usr *usr = new Usr();
    memset(usr, 0, sizeof(usr));

    //对表单的操作
    QSqlQuery query;
    //先准备
    query.prepare("select *from usr where uname = :name");

    query.bindValue(":name",name);

    QString sphone = " ";
    QString pw = " ";
    QString sname = " ";

    if(!query.exec()){
        usr->setName("Do not find");
        instance->removeConnect();
        return *usr;
    }
    else{
        while(query.next())
        {
            sphone = query.value(5).toString();
            pw = query.value(2).toString();
            sname = query.value(1).toString();
        }
        if(sphone == phone)
        {
            usr->setPw(pw);
            usr->setName(sname);
        }
        else
        {
            usr->setName("Phone is wrong");
            instance->removeConnect();
            return *usr;
        }
    }
    instance->removeConnect();
    return *usr;
}

//收到礼物
void UsrControl::getGift(pack_t & pack)
{
    DBHelper* instance = DBHelper::getInstance();
    instance->createConnect();

    int imoney = pack.money;
    int ipoint = pack.point;
    int iid = pack.id;
    int iroomid = pack.roomId;

    UsrControl *uc = UsrControl::getInstance();

    Usr & usr = uc->findUsrById(iid);
    Usr & roomusr = uc->findUsrById(iroomid);

    int tmoney = usr.getTMoney();
    tmoney = tmoney - imoney;//余额减掉
    int point = usr.getPoint();
    point = point + imoney*1000;
    point = point - ipoint;

    int money = usr.getMoney();
    money = money + pack.money;

    pack_t &packGifUsr = pack;
    pack_t &packGifRoomer = pack;

    QSqlQuery query;

    query.prepare("update usr set upoint = :point, utmoney = :tmoney where uid = :id");

    query.bindValue(":point", point);
    query.bindValue(":tmoney", tmoney);
    query.bindValue(":id", iid);

    if(query.exec())
    {
        usr.setMoney(money);
        usr.setTMoney(tmoney);
        usr.setPoint(point);
        packGifUsr.type = TYPE_SENT_FLOWER_SUCCESS_USR;
        packGifUsr.money = money;
        packGifUsr.tmoney = tmoney;
        packGifUsr.point = point;
    }
    else
    {
        qDebug() << "礼物赠送失败";
    }

    QTcpSocket * backSocket = 0;

    backSocket = usr.getUsrSocket();
    backSocket->write((const char *)&packGifUsr, sizeof(packGifUsr));

    query.prepare("update usr set upoint = :point where uid = :id");

    point = roomusr.getPoint();

    point = point + ipoint + 1000*imoney;

    query.bindValue(":point", point);
    query.bindValue(":id", iroomid);

    if(query.exec())
    {
        roomusr.setPoint(point);
        packGifRoomer.type = TYPE_SENT_FLOWER_SUCCESS_ROOMER;
        packGifRoomer.point = point;
    }
    else
    {
        qDebug() << "礼物赠送失败房主";
    }

    backSocket = roomusr.getUsrSocket();
    backSocket->write((const char *)&packGifRoomer, sizeof(packGifRoomer));
}

//断线后返回套接字对应ID
int UsrControl::disconnect(QTcpSocket * socket)
{
    for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
    {
        if(it.value().getUsrSocket() == socket)
        {
            return it.key();
        }
    }
    return 0;
}
