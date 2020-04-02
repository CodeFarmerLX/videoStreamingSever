#ifndef TUBESERVER_H
#define TUBESERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include "package.h"

namespace Ui {
class TubeServer;
}

class TubeServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TubeServer(QWidget *parent = 0);
    ~TubeServer();

    //从数据库将用户信息读入容器
    void loadFromDBtoUsrs();

private slots:
    void on_btnStartLisen_clicked();

    void newConnectionSlot();

    void setText(QString text);

    void on_btnCloseLisen_clicked();

    void sentToClientSlot(pack_t);

    void on_pushButton_clicked();

private:
    Ui::TubeServer *ui;
    QTcpServer* tubeServer;
};

#endif // TUBESERVER_H
