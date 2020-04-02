#include "RoomControl.h"
#include <QDebug>
#include "mysleep.h"
#include "NetWorkHelper.h"

RoomControl* RoomControl::instance = 0;

RoomControl::RoomControl()
{

}

RoomControl* RoomControl::getInstance()
{
    qDebug() << "产生单例指针";
    qDebug() << instance;
    if(0 == instance){
        instance = new RoomControl();
    }
    return instance;
}

//新建房间并插入
void RoomControl::insertRoomMaster(int key, Room& newRoom)
{
    rooms.insert(key, newRoom);
    qDebug() << "新建房间成功:房间名为" << newRoom.getRoomName();
}

//删除房间
void RoomControl::deleteRoom(int key)
{
    QMap<int, Room>::iterator it = rooms.find(key);
    if(rooms.end() == it){
        qDebug() << "删除房间失败（没有找到该房间id）";
    }
    else{
        rooms.erase(it);
        qDebug() << "删除房间成功";
    }
}

//更新房间信息
void RoomControl::updateRoom(QTcpSocket * returnSocket)
{
            qDebug() << "收到刷新房间信息";
    QTcpSocket * clientSocket = returnSocket;

    pack_t packRet;
    memset(&packRet, 0, sizeof(packRet));

    if(rooms.isEmpty()){
        qDebug() << "刷新房间成功——没有房间";
        packRet.type = TYPE_UPDATE_ROOMS_SUCCESS_NO_ROOM;
        clientSocket->write((const char *)&packRet, sizeof(packRet));
    }
    else{
        qDebug() << "刷新房间成功——有房间";
        packRet.type = TYPE_UPDATE_ROOMS_SUCCESS;
        for(QMap<int, Room>::iterator it = rooms.begin(); it != rooms.end(); it++){

            mySleep(100);

            int sid = it.value().getRoomId();
            QString sname = it.value().getUsrName();
            qDebug() << "刷新房间成功——有房间_用户名字" << sname;
            QString sRoomName = it.value().getRoomName();

            QByteArray bName = sname.toLocal8Bit();
            QByteArray bRoomName = sRoomName.toLocal8Bit();

            const char * cName = bName.data();
            const char * cRoomName = bRoomName.data();

            packRet.id = sid;
            qstrcpy(packRet.name, cName);
            qstrcpy(packRet.roomName, cRoomName);

            clientSocket->write((const char *)&packRet, sizeof(packRet));
        }
    }
}

//创建新房间
void RoomControl::createRoom(const pack_t & pack, QTcpSocket * returnSocket)
{
    QTcpSocket * clientSocket = returnSocket;

    qDebug() << "创建新房间的socket peer QHostAddress：" << returnSocket->peerAddress();
    qDebug() << "创建新房间的socket peer Port：" << returnSocket->peerPort();

    int iid = pack.id;
    QString sname = QString::fromLocal8Bit(pack.name);
    QString sroomName = QString::fromLocal8Bit(pack.roomName);
    QHostAddress ipAddress = clientSocket->peerAddress();

    Room *newRoom = new Room();

//    this->setAll(iid,sname);
    newRoom->setRoomId(iid);
    newRoom->setUsrName(sname);
    newRoom->setRoomName(sroomName);
    newRoom->setRoomSocket(returnSocket);
    newRoom->setGroupAddress(ipAddress);

    qDebug() << "创建新房间的房主名字(usr.name)1：" << newRoom->getUsrName();

    this->insertRoomMaster(iid, *newRoom);

    pack_t packRet;
    memset(&packRet, 0, sizeof(packRet));

    //余额不足创建失败

    packRet.type = TYPE_CREATE_ROOM_SUCCESS;
    packRet.id = iid;
    qstrcpy(packRet.name, pack.name);
    qstrcpy(packRet.roomName, pack.roomName);
    qstrcpy(packRet.roomIpAddress, pack.roomIpAddress);
    packRet.tmoney = pack.tmoney;
    packRet.point = pack.point;
    packRet.roomPort = clientSocket->peerPort();
    qDebug() << "创建新房间的port：" << packRet.roomPort;
    clientSocket->write((const char *)&packRet, sizeof(packRet));
//创建房间后刷新所有大厅的房间列表*******************************

    mySleep(100);

    pack_t& packClear = packRet;

    packClear.type = TYPE_UPDATE_ROOMS_SUCCESS;

    QTcpSocket * backSocket = 0;

    UsrControl * uc = UsrControl::getInstance();

    qDebug() << "创建房间后刷新大厅列表调用用户控制单例后";

    QMap<int, Usr> usrs = uc->getUsrs();

    for(QMap<int, Usr>::iterator it = usrs.begin(); it != usrs.end(); it++)
    {
        mySleep(100);
        backSocket = (*it).getUsrSocket();
        backSocket->write((const char *)&packClear, sizeof(packClear));
    }
    mySleep(100);

}

//通过房间ID找到房间
Room& RoomControl::findRoomById(int key)
{
    QMap<int, Room>::iterator it = this->rooms.find(key);

    if(rooms.end() == it)
    {
        qDebug() << "按照ID寻找房间失败";
        Room * room = new Room();
        memset(room, 0, sizeof(room));
        return *room;
    }
    else
    {
        qDebug() << "按照ID寻找房间成功-加入房间";
    }
    return *it;
}

//更新所有连接的套接字房间列表
void RoomControl::updateRoomAll()
{
    pack_t packClear;
    memset(&packClear, 0, sizeof(packClear));

    packClear.type = TYPE_UPDATE_ROOMS_CLEAR_TEXT;

    QTcpSocket * backSocket = 0;

    UsrControl * uc = UsrControl::getInstance();

    QMap<int, Usr> usrs = uc->getUsrs();

    for(QMap<int, Usr>::iterator it = usrs.begin(); it != usrs.end(); it++)
    {
        mySleep(1000);
        backSocket = (*it).getUsrSocket();
        backSocket->write((const char *)&packClear, sizeof(packClear));
        mySleep(1000);
    }
}
