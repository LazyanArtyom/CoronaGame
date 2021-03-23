#include "tcpclient.h"
#include <iostream>
#include <QMessageBox>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40001

CClient::CClient()
{
    //
    /// Init socket and timer
    //
    m_pSocket = new QTcpSocket(this);
    m_pFlushTimer = new QTimer(this);

    //
    /// Make connections
    //
    connect(m_pSocket, &QTcpSocket::connected, this, &CClient::onSocketConnected);
    connect(m_pSocket, &QTcpSocket::readyRead, this, &CClient::onReadyRead);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &CClient::onSocketDisconnected);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CClient::onFlushTimerTick);

    //
    /// Flush data to std::out
    //
    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();
}

bool CClient::isConnected()
{
    return m_pSocket->state() == QAbstractSocket::ConnectedState;
}

void CClient::connectToServer()
{
    if (m_pSocket->state() == QAbstractSocket::UnconnectedState)
    {
        m_pSocket->connectToHost(HOST_ADDRESS, HOST_PORT);
        bool bConnected = m_pSocket->waitForConnected(3000);
        if (!bConnected)
        {
            (void)QMessageBox::critical(nullptr, tr("COVID-19"), tr(m_pSocket->errorString().toStdString().c_str()), QMessageBox::Close);
            exit(0);
        }
    }
}

void CClient::sendMessage(QString msg)
{
    QString msgToSend = msg;
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        emit sigReport("Client ERR: Cannot send; Not connected\n");
        return;
    }

    QByteArray data = msgToSend.toLatin1();
    m_pSocket->write(data);
    m_pSocket->waitForBytesWritten(3000);
}

void CClient::disconnectFromServer()
{
    qDebug() << "state: " << m_pSocket->state();
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        emit sigReport("Client ERR: No connection to close.\n");
        return;
    }

    qDebug() << "Client disconnecting";
    m_pSocket->disconnectFromHost();
}

void CClient::onSocketConnected()
{
    onReadyRead();
}

void CClient::onSocketDisconnected()
{
    emit sigReport("Connection Closed.\n");
}

void CClient::onReadyRead()
{
    qDebug() << "Reading...";
    QByteArray data = m_pSocket->readAll();
    qDebug() << "data from server " << data;
    if (data.isEmpty())
    {
        qDebug() << "Dat is empty";
        return;
    }
    if (data.startsWith("RR"))
    {
        QStringList sValue = QString::fromStdString(data.toStdString()).split(" ");
        qDebug() << sValue;
        switch (sValue[1].toInt())
        {
        case 1:
            emit sigFirstClient();
            break;
        case 2:
            emit sigClientSettings(sValue[2].toInt(), sValue[3].toInt(), sValue[4].toInt(), sValue[5].toInt());
            break;
        default:
            break;
        }
        return;
    }

    QStringList lstValue = QString::fromStdString(data.toStdString()).split(",");
    //
    /// Get message for client
    //
    QString sMessageForPlayer = lstValue.back();
    lstValue.pop_back();

    if (sMessageForPlayer != "YOUR TURN" && sMessageForPlayer != "WAIT FOR THE NEXT PLAYERS")
    {
        //
        /// Name score processing
        //
        qDebug() << "sMessageForPlayer" << sMessageForPlayer;
        QStringList sNameScoreList = sMessageForPlayer.split(".");
        QMap<QString, int> oNameScoreMap;
        for (size_t i = 0; i < sNameScoreList.size(); i +=2)
        {
            oNameScoreMap[sNameScoreList[i]] = sNameScoreList[i + 1].toInt();
        }
        emit sigGameOver(oNameScoreMap);
        return;
    }

    qDebug() << "Value count" << lstValue.size();

    QList<SValue> lstValues;
    for (int i = 0; i < lstValue.count() - 3; i += 4)
    {
        int nRow = lstValue[i].toInt();
        int nCol = lstValue[i + 1].toInt();
        int nValue = lstValue[i + 2].toInt();
        int nPlvac = lstValue[i + 3].toInt();

        qDebug() << "recived from serv : " << nRow << " : " << nCol << " : " << nValue << nPlvac;


        lstValues.push_back(SValue(nRow, nCol, nValue, nPlvac));
    }
    emit sigMessageForPlayer(sMessageForPlayer);
    emit sigUpdateFields(lstValues);
}

void CClient::onFlushTimerTick()
{
    std::flush(std::cout);
}


