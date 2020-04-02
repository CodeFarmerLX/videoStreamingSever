#include "Room.h"
#include <QDebug>

Room::Room()
{

}

//插入进入房间的用户
void Room::insertUsrIntoRoom(int key, Usr & newUsr)
{
    this->usrInRoom.insert(key,newUsr);
    qDebug() << "房间内插入游客成功, 游客的IP为：" << newUsr.getIpAddress();
}

//删除
bool Room::deleteUsrFromRoom(int key)
{
    int id = key;
    QMap<int, Usr>::iterator it = this->usrInRoom.find(id);
    if(usrInRoom.end() == it){
        qDebug() << "删除房间内游客失败（没有找到该游客）";
        return false;
    }
    else{
        usrInRoom.erase(it);
        qDebug() << "删除房间内游客成功";
        return true;
    }
}
