#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H
#include "SocketHandler.h"
#include <QVector>
#include <QMap>
#include <QDebug>
#include "UsrControl.h"


class NetworkHelper
{
private:
    NetworkHelper();
    NetworkHelper(const NetworkHelper&) = delete;
    NetworkHelper& operator = (NetworkHelper&) = delete;
    static NetworkHelper* instance;

    QVector<SocketHandler*> clients;

    QMap<int, SocketHandler*> onlineClients;
//    QMap<int, SocketHandler*> rooms;
public:
//    //从数据库将用户信息读入容器
//    void loadFromDBtoUsrs();

    static NetworkHelper* getInstance();
    //插入任意接入服务器的套接字，不管是否成功登录
    void insertClient(SocketHandler* sh)
    {
        clients.push_back(sh);
    }
    //插入成功登录的套接字，及对应姓名（按照ID插入）
    void insertOnlineClients(int key, SocketHandler* sh)
    {
        onlineClients.insert(key, sh);
        qDebug() << "大厅在线用户插入成功";
    }
//    //插入进入房间的用户姓名及套接字（按照ID插入）
//    void insertRoomMaster(int key, SocketHandler* sh)
//    {
//        rooms.insert(key, sh);
//        qDebug() << "房间创建成功_房主姓名为 ：" << sh->getName();
//        for(QMap<int, SocketHandler*>::iterator it = rooms.begin(); it != rooms.end(); it++){
//            QString sname = it.value()->getName();
//            qDebug() << "房间创建成功后——房间_用户名字" << sname;
//        }
//    }
    //获取任意接入服务器的套接字，不管是否成功登录
    QVector<SocketHandler*>& getClients() { return this->clients; }
    //获取成功登录的套接字，及对应姓名（按照ID插入）
    QMap<int, SocketHandler*> getOnlineClients() { return this->onlineClients; }
    //获取进入房间的用户姓名及套接字（按照ID插入）
//    QMap<int, SocketHandler*> getRooms() { return this->rooms; }

    //删除任意接入服务器的套接字，不管是否成功登录
    void deleteClient(int id);
    //删除成功登录的套接字，及对应姓名（按照ID插入）
    void deleteOnlineClient(int id);
//    //删除进入房间的用户姓名及套接字（按照ID插入）
//    void deleteRoomMaster(int id);
};

#endif // NETWORKHELPER_H
