#include "SocketHandler.h"
#include <QDebug>
#include "NetworkHelper.h"
#include "package.h"
#include "Usr.h"
#include "UsrControl.h"
#include <QHostAddress>
#include "socketsleep.h"

SocketHandler::SocketHandler(QObject *parent) : QObject(parent)
{

}


SocketHandler::SocketHandler(QTcpSocket *clientSocket,QObject *parent)
    :clientSocket(clientSocket),
      QObject(parent)
{
    //客户端套接字  readRead信号(当套接字中有数据时，会触发这个信号)
    connect(clientSocket, SIGNAL(readyRead()),
            this, SLOT(readReadSlot()));
    this->usrId = 0;
    this->usrName = "初始化testName";

    connect(this->clientSocket, SIGNAL(disconnected()),
            this, SLOT(disconnectSlot()));
}

SocketHandler::~SocketHandler()
{

}

//断线后的操作
void SocketHandler::disconnectSlot()
{
    UsrControl * uc = UsrControl::getInstance();
    int key = uc->disconnect(this->clientSocket);

    qDebug() << key << "断开连接";

    if(0 == key)
    {
    }
    else
    {
        RoomControl * rc = RoomControl::getInstance();
        qDebug()<<"获得单例后";

        Usr & usr = uc->findUsrById(key);
        qDebug()<<"寻找用户后";

        uc->logOutState(usr.getName());
        uc->deleteUsrFromUsrs(key);

        QMap<int, Room> & rooms = rc->getRooms();
        qDebug()<<"获得房间后";

        if(rooms.isEmpty())
        {
            qDebug() << "没有房间";
        }
        else
        {
            pack_t packRet;
            memset(&packRet, 0, sizeof(packRet));

            QTcpSocket * backSocket = 0;

            for(QMap<int, Room>::iterator it = rooms.begin(); it != rooms.end(); it++)
            {
                qDebug()<<"进入循环房间删除";
                if((*it).deleteUsrFromRoom(key))
                {
                    Room & room = *it;
                    QMap<int, Usr> Usrs = room.getUsrInRoom();

                    packRet.type = TYPE_EXIT_ROOM_SUCCESS_CLEAR_USR;
                    for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
                    {
                        backSocket = (*it).getUsrSocket();
                        backSocket->write((const char *)&packRet, sizeof(packRet));
                    }
                    backSocket = room.getRoomSocket();
                    backSocket->write((const char *)&packRet, sizeof(packRet));

                    socketSleep(100);

                    packRet.type = TYPE_EXIT_ROOM_SUCCESS_UPDATE_USR;
                    for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
                    {
                        backSocket = (*it).getUsrSocket();
                        QString sname = (*it).getName();
                        QByteArray bname = sname.toLocal8Bit();
                        const char * cname = bname.data();
                        qstrcpy(packRet.name, cname);
                        backSocket->write((const char *)&packRet, sizeof(packRet));
                        backSocket = room.getRoomSocket();
                        backSocket->write((const char *)&packRet, sizeof(packRet));
                    }
                }
            }

            Room & room = rc->findRoomById(key);

            if(room.getRoomId() == 0)
            {

            }
            else
            {
                packRet.type = TYPE_CLOSE_ROOM_SUCCESS_FOR_ROOMER;
                room.getRoomSocket()->write((const char *)&packRet, sizeof(packRet));

                QMap<int, Usr> usrs = room.getUsrInRoom();

                packRet.type = TYPE_CLOSE_ROOM_SUCCESS_FOR_USR;

                for(QMap<int, Usr>::iterator it = usrs.begin(); it != usrs.end(); it++)
                {
                    backSocket = (*it).getUsrSocket();
                    backSocket->write((const char *)&packRet, sizeof(packRet));
                }

                rc->deleteRoom(key);
            }
        }
    }
}


void SocketHandler::readReadSlot()
{
    pack_t pack;
    memset(&pack, 0, sizeof(pack));

    while(!this->clientSocket->atEnd()){

        this->clientSocket->read((char*)&pack, sizeof(pack));
        qDebug() << "type: " << pack.type << ", name: "<< pack.name
                    << ", pwd: " << pack.pwd << ", phone: " << pack.phone;

        switch (pack.type) {
        case TYPE_REGIST://收到注册信息
        //注册函数 将我们用户信息保存到数据库中并将结果反馈给客户端.
            usrRegist(pack);
            break;
        case TYPE_LOGIN://收到登录信息
            qDebug() << "收到登录信息";
            usrLogin(pack);
            break;
        case TYPE_LOGOUT://收到登出并关闭程序信息
            logOut(pack);
            break;
        case TYPE_LOGOUT_BACK://收到登出返回上一界面信息
            logOutBack(pack);
            break;
        case TYPE_UPDATE_ROOMS://收到刷新房间信息
            updateRoom(this->clientSocket);
            break;
        case TYPE_UPDATE_ROOMS_ALL:
            updateRoomAll();
            break;
        case TYPE_CREATE_ROOM://收到创建新房间信息
            createRoom(pack, this->clientSocket);
            break;
        case TYPE_JOIN_ROOM://收到加入房间信息
            joinRoom(pack);
            break;
        case TYPE_SENT_TEXT_IN_ROOM:
            sentMessageToUsrInRoom(pack);
            break;
        case TYPE_EXIT_ROOM:
            exitRoom(pack);
            break;
        case TYPE_CLOSE_ROOM:
            closeRoom(pack);
            break;
        case TYPE_UP_USR_INFO:
            upUsrInfo(pack);
            break;
        case TYPE_RECHARGE:
            reMoney(pack);
            break;
        case TYPE_FIND_PW:
            findPw(pack);
            break;
        case TYPE_SENT_FLOWER:
            getGift(pack);
            break;
        default:
            break;
        }
    }

}

//注册
void SocketHandler::usrRegist(const pack_t &pack)
{
    QString name = QString::fromLocal8Bit(pack.name);
    QString pw = QString::fromLocal8Bit(pack.pwd);
    QString phone = QString::fromLocal8Bit(pack.phone);

    Usr * regUsr = new Usr();
    memset(regUsr, 0, sizeof(regUsr));

    regUsr->setName(name);
    regUsr->setPw(pw);
    regUsr->setPhone(phone);


    UsrControl * uc = UsrControl::getInstance();
    bool ret = uc->insertUsr(*regUsr);
    qDebug() << ret;

    //将插入结果告诉客户端
    pack_t packRet;
    memset(&packRet, 0, sizeof(packRet));

    if(ret){
//        int id = uc->getIdByName(name);
//        regUsr->setId(id);

//        uc->insertUsrIntoUsrs(id, *regUsr);

        packRet.type = TYPE_REGIST_SUCCESS;
    }
    else{        
        delete regUsr;
        packRet.type = TYPE_REGIST_FAIL;
    }
    this->clientSocket->write((const char*)&packRet,
                                            sizeof(packRet));
    qDebug() << this->clientSocket->localAddress();
    qDebug() << "发送信号";
}

//登录
void SocketHandler::usrLogin(const pack_t &pack)
{
    QString name = QString::fromLocal8Bit(pack.name);
    QString pw = QString::fromLocal8Bit(pack.pwd);

    UsrControl * uc = UsrControl::getInstance();
    Usr findedUsr = uc->logByName(name, pw);

    QString name1 = findedUsr.getName();

    //将插入结果告诉客户端
    pack_t packRet;
    memset(&packRet, 0, sizeof(packRet));

    if("Don not find" == name1){
        packRet.type = TYPE_LOGIN_FAIL_NAME;
    }
    else if ("Pw is false" == name1){
        packRet.type = TYPE_LOGIN_FAIL_PW;
    }
    else if(name == name1){
        if(uc->onlineState(name)){
            NetworkHelper* instance = NetworkHelper::getInstance();
            UsrControl * uc = UsrControl::getInstance();

            Usr *usr = new Usr();
            memset(usr, 0, sizeof(usr));

            packRet.type = TYPE_LOGIN_SUCCESS;
            packRet.id = findedUsr.getId();

            QByteArray qName = findedUsr.getName().toLocal8Bit();
            const char* cName = qName.data();
            qstrcpy(packRet.name, cName);
            packRet.tmoney = findedUsr.getTMoney();
            packRet.money = findedUsr.getMoney();
            packRet.point = findedUsr.getPoint();

            int id = findedUsr.getId();

            this->setId(id);
            this->setName(findedUsr.getName());

            usr->setId(id);
            usr->setName(findedUsr.getName());
            usr->setTMoney(findedUsr.getTMoney());
            usr->setMoney(findedUsr.getMoney());
            usr->setPhone(findedUsr.getPhone());
            usr->setPoint(findedUsr.getPoint());
            usr->setUsrSocket(this->clientSocket);
            usr->setIpAddress(this->clientSocket->peerAddress());

            uc->insertUsrIntoUsrs(id, *usr);
        }
        else{
            qDebug() << "更改上线状态失败";
            packRet.type = TYPE_LOGIN_FAIL_ONLINE;
        }
    }
    else{}

    this->clientSocket->write((const char*)&packRet,
                                            sizeof(packRet));
}

//登出
void SocketHandler::logOut(const pack_t &pack)
{
     QString name = QString::fromLocal8Bit(pack.name);
     int id = pack.id;

     UsrControl * uc = UsrControl::getInstance();
     pack_t packRet;
     memset(&packRet, 0, sizeof(packRet));

     bool ret = uc->logOutState(name);

     if(ret){
         qDebug() << "更改下线状态成功";
         NetworkHelper* instance = NetworkHelper::getInstance();
         instance->deleteOnlineClient(id);
         UsrControl* uinstance = UsrControl::getInstance();
         uinstance->deleteUsrFromUsrs(id);
         packRet.type = TYPE_LOGOUT_SUCCESS;
     }
     else{
         qDebug() << "更改下线状态失败";
         packRet.type = TYPE_LOGOUT_FAIL;
     }

     this->clientSocket->write((const char*)&packRet,
                                             sizeof(packRet));
}

//登出返回
void SocketHandler::logOutBack(const pack_t &pack)
{
     QString name = QString::fromLocal8Bit(pack.name);
     int id = pack.id;

     UsrControl * uc = UsrControl::getInstance();
     pack_t packRet;
     memset(&packRet, 0, sizeof(packRet));

     bool ret = uc->logOutState(name);

     if(ret){
         qDebug() << "更改下线状态成功";
         NetworkHelper* instance = NetworkHelper::getInstance();
         instance->deleteOnlineClient(id);
         UsrControl* uinstance = UsrControl::getInstance();
         uinstance->deleteUsrFromUsrs(id);
         packRet.type = TYPE_LOGOUT_SUCCESS_BACK;
     }
     else{
         qDebug() << "更改下线状态失败";
         packRet.type = TYPE_LOGOUT_FAIL;
     }

     this->clientSocket->write((const char*)&packRet,
                                             sizeof(packRet));
}

//更新房间信息
void SocketHandler::updateRoom(QTcpSocket * returnSocket)
{
    qDebug() << "socket里的刷新房间函数";
    RoomControl *rc = RoomControl::getInstance();
    rc->updateRoom(returnSocket);
}

//创建新房间
void SocketHandler::createRoom(const pack_t & pack, QTcpSocket * returnSocket)
{
      RoomControl *rc = RoomControl::getInstance();
      qDebug() << "socket里调用rc里的createRoom之前";
      rc->createRoom(pack, returnSocket);
}

//加入房间
void SocketHandler::joinRoom(pack_t & pack)
{
    int usrid = pack.id;
    int roomid = pack.roomId;

    UsrControl * uc = UsrControl::getInstance();
    RoomControl * rc = RoomControl::getInstance();

    qDebug() << "要加入的房间ID : " << roomid;
    Room& room = rc->findRoomById(roomid);
    qDebug() << "找到房间后";
    Usr & usr = uc->findUsrById(usrid);

    pack_t packRet;
    memset(&packRet, 0, sizeof(packRet));

    if(roomid == room.getRoomId())
    {
        room.insertUsrIntoRoom(usrid, usr);
        qDebug() << "被加入房间的IP ：" << room.getGroupAddress();

        /*
        加入房间成功后需要返回的包内的内容：
        0.数据包类型
        1.游客ID
        2.游客姓名
        3.游客余额
        4.游客积分
        5.房间ID
        6.房间名称
        7.房间IP
        */

        packRet.type = TYPE_JOIN_ROOM_SUCCESS;
        packRet.id = usrid;

        QString sname = usr.getName();
        QByteArray bname = sname.toLocal8Bit();
        const char * cname = bname.data();
        qstrcpy(packRet.name, cname);

        packRet.tmoney = usr.getTMoney();
        packRet.point = usr.getPoint();

        packRet.roomId = room.getRoomId();

        QString sroomname = room.getRoomName();
        QByteArray broomname = sroomname.toLocal8Bit();
        const char * croomname = broomname.data();
        qstrcpy(packRet.roomName, croomname);

        QString sroomip = room.getGroupAddress().toString();
        QByteArray broomip = sroomip.toLocal8Bit();
        const char * croomip = broomip.data();
        qstrcpy(packRet.roomIpAddress, croomip);
        packRet.roomPort = room.getRoomSocket()->peerPort();

        this->clientSocket->write((const char *)& packRet, sizeof(packRet));

        socketSleep(100);

//加入房间后其他房间刷新用户信息    *****************************************************
        room = rc->findRoomById(roomid);

        QTcpSocket * backSocket = 0;

        packRet.type = TYPE_EXIT_ROOM_SUCCESS_CLEAR_USR;


        backSocket = room.getRoomSocket();
        backSocket->write((const char *)&packRet, sizeof(packRet));

        QMap<int, Usr> Usrs = room.getUsrInRoom();

        packRet.type = TYPE_EXIT_ROOM_SUCCESS_CLEAR_USR;
        for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
        {
            backSocket = (*it).getUsrSocket();
            backSocket->write((const char *)&packRet, sizeof(packRet));
        }
        backSocket = room.getRoomSocket();
        backSocket->write((const char *)&packRet, sizeof(packRet));

        socketSleep(100);

        packRet.type = TYPE_EXIT_ROOM_SUCCESS_UPDATE_USR;
        for(QMap<int, Usr>::iterator fit = Usrs.begin(); fit != Usrs.end(); fit++)
        {
            backSocket = (*fit).getUsrSocket();
            for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
            {
                QString sname = (*it).getName();
                QByteArray bname = sname.toLocal8Bit();
                const char * cname = bname.data();
                qstrcpy(packRet.name, cname);
                backSocket->write((const char *)&packRet, sizeof(packRet));

                socketSleep(100);
            }
            backSocket = room.getRoomSocket();
            QString sname = (*fit).getName();
            QByteArray bname = sname.toLocal8Bit();
            const char * cname = bname.data();
            qstrcpy(packRet.name, cname);
            backSocket->write((const char *)&packRet, sizeof(packRet));

            socketSleep(100);
        }
    }
    else
    {
        packRet.type = TYPE_JOIN_ROOM_FAIL;
        this->clientSocket->write((const char *)&packRet, sizeof(packRet));
    }
}

//收到来自房间的信息并群发出去
void SocketHandler::sentMessageToUsrInRoom(pack_t & pack)
{
    qDebug() << "收到来自房间的信息：" << pack.message;

    pack_t packBackM;
    memset(&packBackM, 0, sizeof(packBackM));
    packBackM.type = TYPE_GET_TEXT_IN_ROOM;
    qstrcpy(packBackM.message, pack.message);
    qstrcpy(packBackM.name, pack.name);

    int roomid = pack.roomId;


    RoomControl * rc = RoomControl::getInstance();
    Room & room = rc->findRoomById(roomid);
    QMap<int, Usr>& usrsInRoom = room.getUsrInRoom();

    QTcpSocket * socketBack = room.getRoomSocket();
    qDebug() << "发送到房间的socket ：" << socketBack;
    socketBack->write((const char *)&packBackM, sizeof(packBackM));


    for(QMap<int, Usr>::iterator it = usrsInRoom.begin(); it != usrsInRoom.end(); it++)
    {
        socketBack = (*it).getUsrSocket();
        socketBack->write((const char *)&packBackM, sizeof(packBackM));
        qDebug() << "发送到房间用户的socket ：" << socketBack;
    }
}

//更新所有连接的套接字房间列表
void SocketHandler::updateRoomAll()
{
    RoomControl *rc = RoomControl::getInstance();
    rc->updateRoomAll();
}

//退出房间
void SocketHandler::exitRoom(pack_t & pack)
{
    pack_t & packRet = pack;

    int id = pack.id;
    int roomid = pack.roomId;

    RoomControl * rc = RoomControl::getInstance();

    Room & room = rc->findRoomById(roomid);
    QTcpSocket * backSocket = 0;

    if(room.deleteUsrFromRoom(id))
    {
        packRet.type = TYPE_EXIT_ROOM_SUCCESS;
        this->clientSocket->write((const char *)&packRet, sizeof(packRet));

        QMap<int, Usr> Usrs = room.getUsrInRoom();

        packRet.type = TYPE_EXIT_ROOM_SUCCESS_CLEAR_USR;
        for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
        {
            backSocket = (*it).getUsrSocket();
            backSocket->write((const char *)&packRet, sizeof(packRet));
        }
        backSocket = room.getRoomSocket();
        backSocket->write((const char *)&packRet, sizeof(packRet));

        socketSleep(100);

        packRet.type = TYPE_EXIT_ROOM_SUCCESS_UPDATE_USR;
        for(QMap<int, Usr>::iterator it = Usrs.begin(); it != Usrs.end(); it++)
        {
            backSocket = (*it).getUsrSocket();
            QString sname = (*it).getName();
            QByteArray bname = sname.toLocal8Bit();
            const char * cname = bname.data();
            qstrcpy(packRet.name, cname);
            backSocket->write((const char *)&packRet, sizeof(packRet));
            backSocket = room.getRoomSocket();
            backSocket->write((const char *)&packRet, sizeof(packRet));
        }

        socketSleep(100);
    }
    else
    {
        packRet.type = TYPE_EXIT_ROOM_FAIL;
        this->clientSocket->write((const char *)&packRet, sizeof(packRet));
    }
}

//关闭房间
void SocketHandler::closeRoom(pack_t & pack)
{
    int roomid = pack.roomId;

    RoomControl * rc = RoomControl::getInstance();

    Room & room = rc->findRoomById(roomid);

    pack_t & packRet = pack;
    packRet.type = TYPE_CLOSE_ROOM_SUCCESS_FOR_ROOMER;
    this->clientSocket->write((const char *)&packRet, sizeof(packRet));

    QMap<int, Usr> usrs = room.getUsrInRoom();
    QTcpSocket * backSocket = 0;
    packRet.type = TYPE_CLOSE_ROOM_SUCCESS_FOR_USR;

    for(QMap<int, Usr>::iterator it = usrs.begin(); it != usrs.end(); it++)
    {
        backSocket = (*it).getUsrSocket();
        backSocket->write((const char *)&packRet, sizeof(packRet));
    }

    rc->deleteRoom(roomid);
}

//刷新个人信息
void SocketHandler::upUsrInfo(pack_t & pack)
{
    int id = pack.id;

    UsrControl * uc = UsrControl::getInstance();
    Usr & usr = uc->findUsrById(id);

    pack_t & packRet = pack;

    packRet.type = TYPE_BACK_USR_INFO;

    QString sname = usr.getName();
    QByteArray bname = sname.toLocal8Bit();
    const char * cname = bname.data();
    qstrcpy(packRet.name, cname);

    packRet.tmoney = usr.getTMoney();
    packRet.point = usr.getPoint();

    this->clientSocket->write((const char *)&packRet, sizeof(packRet));
}

//充值
void SocketHandler::reMoney(pack_t & pack)
{
    int iid = pack.id;
    int remoney = pack.money;

    UsrControl *uc = UsrControl::getInstance();
    Usr & usr = uc->findUsrById(iid);

    int tmoney = usr.getTMoney();

    tmoney = tmoney + remoney;

    DBHelper * db = DBHelper::getInstance();

    db->createConnect();

    QSqlQuery query;

    query.prepare("update usr set utmoney = :tmoney where uid = :id");

    query.bindValue(":tmoney", tmoney);
    query.bindValue(":id", iid);

    pack_t & packRet = pack;

    if(query.exec()){

        usr.setTMoney(tmoney);

        packRet.type = TYPE_RECHARGE_SUCCESS;

        packRet.tmoney = tmoney;
    }
    else
    {
        packRet.type = TYPE_RECHARGE_FAIL;
    }

    this->clientSocket->write((const char *)&packRet, sizeof(packRet));
}

//找回密码
void SocketHandler::findPw(pack_t & pack)
{
    UsrControl * uc = UsrControl::getInstance();
    Usr & usr = uc->findPw(pack);
    pack_t & packRet = pack;
    if("Do not find" == usr.getName())
    {
        packRet.type = TYPE_FIND_PW_FAIL_NAME;
        this->clientSocket->write((const char *)&packRet, sizeof(packRet));
        return;
    }
    else if("Phone is wrong" == usr.getName())
    {
        packRet.type = TYPE_FIND_PW_FAIL_PHONE;
        this->clientSocket->write((const char *)&packRet, sizeof(packRet));
        return;
    }
    else
    {
        packRet.type = TYPE_FIND_PW_SUCCESS;

        QString spw = usr.getPw();
        QByteArray bpw = spw.toLocal8Bit();
        const char * cpw = bpw.data();

        qstrcpy(packRet.pwd, cpw);
    }


    this->clientSocket->write((const char *)&packRet, sizeof(packRet));
}

//获得礼物
void SocketHandler::getGift(pack_t & pack)
{
    UsrControl * uc = UsrControl::getInstance();
    uc->getGift(pack);
}
