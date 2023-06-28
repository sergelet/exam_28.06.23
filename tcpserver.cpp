#include "tcpserver.h"
#include <QDebug>
#include <QCoreApplication>

tcpserver::~tcpserver()
{
    mTcpServer->close();
    //server_status=0;
}
tcpserver::tcpserver(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &tcpserver::slotNewConnection);
    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void tcpserver::slotNewConnection(){

    if (connectedClients >= 6) {
        // Превышено допустимое количество клиентов
        // Можно отправить сообщение или просто игнорировать подключение
        return;
    }
    QTcpSocket * curr_Socket;
    curr_Socket = mTcpServer->nextPendingConnection();
    connect(curr_Socket, &QTcpSocket::readyRead, this, &tcpserver::slotServerRead);
    connect(curr_Socket, &QTcpSocket::disconnected, this, &tcpserver::slotClientDisconnected);
    // Увеличить счетчик подключенных клиентов
    connectedClients++;
    mTcpSocket.append(curr_Socket);
}


void tcpserver::slotServerRead(){


    QByteArray array;
    QString quest,ans;
    int k=0;
    QTcpSocket *curr_Socket = (QTcpSocket*)sender();
    while(curr_Socket->bytesAvailable()>0)
    {

        array = curr_Socket->readAll();
        quest.append(array);
    }
    array.clear();
    for(int i = 0; i<mTcpSocket.size();i++){
        if(curr_Socket == mTcpSocket.at(i)){
           break;
        }
        k++;
    }
    ans = quest.trimmed();
    for (int i = 2; i < quest.size();i++) {
        if((i+1)%3==0)
        {
            ans.append(quest.at(i));
        }
    }
    sendToClient(k, ans + "\n\r");

}

void tcpserver::slotClientDisconnected(){
    QTcpSocket * disSoc = (QTcpSocket*)sender();
        for(int i = 0; i<mTcpSocket.size();i++){
            if(disSoc == mTcpSocket.at(i)){
                mTcpSocket.removeAt(i);
                connectedClients--;
                break;
            }
        }
}
void tcpserver::sendToClient(int pos,QString otvet)
{
    for(int i = 0; i<mTcpSocket.size();i++)
    {
        if (i != pos){
            mTcpSocket[i]->write(otvet.toUtf8());
        }

    }

}
