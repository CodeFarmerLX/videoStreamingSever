#include "TubeServer.h"
#include "ui_TubeServer.h"
#include <QHostAddress>
#include <QTcpSocket>
#include "SocketHandler.h"
#include "NetworkHelper.h"
#include <QDebug>


TubeServer::TubeServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TubeServer)
{
    ui->setupUi(this);
    //1 创建套接字管理对象
    this->tubeServer = new QTcpServer(this);

    //3 链接newConnection信号
    connect(this->tubeServer, SIGNAL(newConnection()),
                        this, SLOT(newConnectionSlot()));
}

TubeServer::~TubeServer()
{
    delete ui;
    delete tubeServer;
}

void TubeServer::loadFromDBtoUsrs()
{
    UsrControl * uc = UsrControl::getInstance();

    Usr* newUsr = new Usr();
    memset(newUsr, 0, sizeof(newUsr));

    DBHelper * open =  DBHelper::getInstance();
    open->createConnect();

    QSqlQuery query;

    bool ret = query.exec("select * from usr");
    if(ret)
    {
        while(query.next())
        {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
//            QString pw = query.value(2).toString();
            int tmoney = query.value(3).toInt();
            int money = query.value(4).toInt();
            QString phone = query.value(5).toString();
            int point = query.value(6).toInt();

            newUsr->setId(id);
            newUsr->setName(name);
            newUsr->setTMoney(tmoney);
            newUsr->setMoney(money);
            newUsr->setPhone(phone);
            newUsr->setPoint(point);

            uc->insertUsrIntoUsrs(id,*newUsr);
        }
    }

    open->removeConnect();
}

//新的连接信号槽
void TubeServer::newConnectionSlot()
{
    //获取到客户端的信息
    QTcpSocket *clientSocket = this->tubeServer->nextPendingConnection();
    QString ip = clientSocket->peerAddress().toString();
    QString port = QString::number(clientSocket->peerPort());
    QString info = "ip: " + ip + ", port: " + port + " 有新的客户端链接上服务器";
    ui->textEdit->append(info);

    //创建一个类来处理客户端与服务器的通信
    SocketHandler * sh = new SocketHandler(clientSocket, this);

    NetworkHelper* instance = NetworkHelper::getInstance();
    instance->insertClient(sh);
}

void TubeServer::sentToClientSlot(pack_t pack)
{
    qDebug() << "接收信号1";
    QTcpSocket * thisClient = this->tubeServer->nextPendingConnection();
    qDebug() << "发送信号2";
    thisClient->write((const char*)&pack, sizeof(pack_t));
}

void TubeServer::setText(QString text)
{
    ui->textEdit->append(text);
}


void TubeServer::on_btnStartLisen_clicked()
{
    ui->textEdit->clear();

    if(!this->tubeServer->isListening()){
        //2 监听
        bool ret = tubeServer->listen(QHostAddress::LocalHost, 9090);
        if(ret){
            ui->textEdit->clear();
            ui->textEdit->append("监听成功");
        }else{
            ui->textEdit->clear();
            ui->textEdit->append("监听失败");
        }
    }else{
        ui->textEdit->clear();
        ui->textEdit->append("已经启动监听");
    }
}

void TubeServer::on_btnCloseLisen_clicked()
{
    this->tubeServer->close();
    if(!this->tubeServer->isListening()){
        ui->textEdit->clear();
        ui->textEdit->append("监听已停止");
    }
    else{
        ui->textEdit->clear();
        ui->textEdit->append("停止失败");
    }
}

void TubeServer::on_pushButton_clicked()
{
    RoomControl* instance = RoomControl::getInstance();
    QMap<int, Room> rooms = instance->getRooms();

    for(QMap<int, Room>::iterator it = rooms.begin(); it != rooms.end(); it++){

        QString sname = it.value().getRoomName();
        qDebug() << "服务器检查-房间_用户名字" << sname<< " : " << it.value().getRoomId();
        }
}
