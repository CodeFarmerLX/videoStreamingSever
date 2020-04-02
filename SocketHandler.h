#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include "package.h"
#include <QMap>
#include "Usr.h"
#include "RoomControl.h"

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit SocketHandler(QObject *parent = 0);
    explicit SocketHandler(QTcpSocket* clientSocket,
                                        QObject *parent = 0);
    ~SocketHandler();

    QTcpSocket* getSocket() { return this->clientSocket; }
    QString& getName(){ return this->usrName; }
    int getId(){ return this->usrId; }
//    QMap<int, SocketHandler*> getClientInRoom() {return this->clientsInRoom; }
//    QString& getRoomName() { return this->roomName; }

//    void setAll(int id = 0, const QString & name = "testName", const QString & pw = " ",
//                int tmoney = 0, int money = 0, const QString & phone = " ", int point = 0);
    void setId(int id) { this->usrId = id; }
    void setName(const QString & name){ this->usrName = name; }
//    void setRoomName(const QString & roomName){ this->roomName = roomName; }

//    void insetClientToRoom(int key, SocketHandler* cInRoom)
//    {
//        this->clientsInRoom.insert(key, cInRoom);
//    }

    void usrRegist(const pack_t&pack);//注册函数
    void usrLogin(const pack_t&pack);//登录函数
    void logOut(const pack_t &pack);//登出关闭函数
    void logOutBack(const pack_t &pack);//登出返回函数
    void updateRoom(QTcpSocket * returnSocket);//刷新房间信息函数
    void createRoom(const pack_t & pack, QTcpSocket * returnSocket);//创建新房间

    //创建新房间
    void createRoom(const pack_t & pack);
    //加入房间
    void joinRoom(pack_t & pack);
    //收到来自房间的信息并群发出去
    void sentMessageToUsrInRoom(pack_t & pack);
    //更新所有连接的套接字房间列表
    void updateRoomAll();
    //退出房间
    void exitRoom(pack_t & pack);
    //关闭房间
    void closeRoom(pack_t & pack);
    //刷新个人信息
    void upUsrInfo(pack_t & pack);
    //充值
    void reMoney(pack_t & pack);
    //找回密码
    void findPw(pack_t & pack);

    void getGift(pack_t & pack);
signals:
//    void textToWidget(QString);
//    void sentToClient(pack_t);
public slots:
    void readReadSlot();

    void disconnectSlot();

private:
    QTcpSocket* clientSocket;
    QString     usrName;
    int         usrId;
//    QString     roomName;
//    Usr *       usr;//房主或游客信息
//    QMap<int, SocketHandler*> clientsInRoom;

//    QMap<QString, QTcpSocket*> sockets;
};

#endif // SOCKETHANDLER_H
