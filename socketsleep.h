#ifndef SOCKETSLEEP_H
#define SOCKETSLEEP_H
#include <QApplication>
#include <QTime>

void socketSleep(unsigned int msec);

void socketSleep(unsigned int msec){

    QTime reachTime =  QTime::currentTime().addMSecs(msec);

    while(QTime::currentTime() < reachTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
#endif // SOCKETSLEEP_H
