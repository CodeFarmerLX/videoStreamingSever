#ifndef ROOMCONTROL_H
#define ROOMCONTROL_H
#include "Room.h"

class RoomControl
{
private:    
    RoomControl();
    RoomControl& operator = (const RoomControl& room) = delete;
    RoomControl(const RoomControl& room) = delete;
    static RoomControl * instance;
    QMap<int, Room> rooms;
public:
    static RoomControl* getInstance();

    QMap<int, Room> & getRooms(){ return this->rooms; }
    void insertRoomMaster(int key, Room& newRoom);

    void updateRoom(QTcpSocket * returnSocket);
    //创建新房间
    void createRoom(const pack_t & pack, QTcpSocket * returnSocket);
    //删除房间-退出房间
    void deleteRoom(int key);
    //通关房间ID找到房间
    Room& findRoomById(int key);
    //更新所有连接的套接字房间列表
    void updateRoomAll();
};

#endif // ROOMCONTROL_H
