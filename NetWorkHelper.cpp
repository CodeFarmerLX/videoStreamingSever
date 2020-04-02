#include "NetworkHelper.h"


NetworkHelper::NetworkHelper()
{
}

NetworkHelper* NetworkHelper::instance = 0;

NetworkHelper* NetworkHelper::getInstance()
{
    if(instance == 0)
        instance = new NetworkHelper();
    return instance;
}

//删除任意接入服务器的套接字，不管是否成功登录
void NetworkHelper::deleteClient(int id)
{

}

//删除成功登录的套接字，及对应姓名（按照ID插入）
void NetworkHelper::deleteOnlineClient(int id)
{
    QMap<int, SocketHandler*>::iterator it = this->onlineClients.find(id);
    if(it == this->onlineClients.end()){
        qDebug() << "大厅在线用户删除失败";
    }
    else{
        this->onlineClients.erase(it);
        qDebug() << "大厅在线用户删除成功";
    }
}

