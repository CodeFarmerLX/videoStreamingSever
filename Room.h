#ifndef ROOM_H
#define ROOM_H

#include "Usr.h"
#include <QTcpSocket>
#include <QHostAddress>
#include "package.h"



class Room
{
private:

    int             roomId;
    QString         usrName;			//房主名
    QString         roomName;
    QTcpSocket*     roomSocket;			//房主的套接字
    QHostAddress	groupAdress;        //广播组地址
//    uint32 groupPort;					//广播组端口

    QMap<int, Usr> usrInRoom;

public:
    Room();

    void setRoomId(int roomid) {this->roomId = roomid; }
    void setUsrName(const QString & usrname){ this->usrName = usrname; }
    void setRoomName(const QString & roomname){ this->roomName = roomname; }
    void setRoomSocket(QTcpSocket* roomsocket) { this->roomSocket = roomsocket; }
    void setGroupAddress(QHostAddress groupaddress) { this->groupAdress = groupaddress; }

    QString getUsrName(){ return this->usrName; }
    QString getRoomName(){ return this->roomName; }
    QTcpSocket* getRoomSocket(){ return this->roomSocket; }
    QHostAddress getGroupAddress(){ return this->groupAdress; }
    QMap<int, Usr> & getUsrInRoom(){ return this->usrInRoom; }
    int getRoomId(){ return this->roomId; }

    void insertUsrIntoRoom(int key, Usr & newUsr);//插入
    bool deleteUsrFromRoom(int key);//删除


};

#endif // ROOM_H
