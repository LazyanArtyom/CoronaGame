#include "tcpserver.h"

#include <iostream>
#include "logic.h"
#include <map>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40001
#define MAX_SOCKETS (4)

std::map<QString, int> sRequestTypeMap = {std::make_pair(QString("RR"), 0), std::make_pair(QString("SGR"), 1), std::make_pair(QString("BGR"), 2), std::make_pair(QString("DGR"), 3), std::make_pair(QString("NRR"), 4)};

CGameServer::CGameServer(QObject* parent):
    QTcpServer(parent)
{
    //
    /// Setup timer and connect to host
    //
    m_pFlushTimer = new QTimer(this);
    if (!listen(QHostAddress::Any, HOST_PORT))
    {
        std::cout << "Failed to listen" << std::endl;
        return;
    }
    //
    std::cout << "Listening on " << serverAddress().toString().toStdString() << ":" << HOST_PORT << std::endl;

    connect(this, &QTcpServer::newConnection, this, &CGameServer::onNewConnection);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CGameServer::onFlushTimerTick);

    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();
    //
    /// init settings
    //
    m_oGameSettins.m_nColCount = 0;
    m_oGameSettins.m_nRowCount = 0;
    m_oGameSettins.m_nGridType = 0;
}

void CGameServer::sendMessage(QString sMessage, int user)
{
    m_users[user]->write(sMessage.toLatin1());
    m_users[user]->flush();
    m_users[user]->waitForBytesWritten(3000);
}

void CGameServer::onNewConnection()
{
    if (m_oServerClients.size() >= m_oGameSettins.m_nPlayerCount)
    {
        std::cout << "Cannot form new connection. Server busy";

        QTcpSocket* pCurSocket = nextPendingConnection();
        pCurSocket->close();
        return;
    }

    QTcpSocket* pCurSocket = nextPendingConnection();
    connect(pCurSocket, &QTcpSocket::readyRead, this, &CGameServer::onReadyRead);
    connect(pCurSocket, &QTcpSocket::disconnected, this, &CGameServer::onDisconnected);

    qDebug() << "Client " << pCurSocket->socketDescriptor();
    m_oServerClients.insert(m_nUserCount, !bool(m_oServerClients.size()));
    m_users.insert(m_nUserCount, pCurSocket);

    if (0 == m_nUserCount)
    {
        QString data = "RR " + QString::number(1);
        //
        /// Send data to main clinet
        //
        sendMessage(data, m_nUserCount);
    }

    std::cout << "Connection formed with user: " << m_nUserCount << "\n";
    ++m_nUserCount;
    return;
}

void CGameServer::onReadyRead()
{
    QTcpSocket* pSocket = (QTcpSocket *)sender();
    //
    /// IF client is inactive skip data processing
    /// Do not chagne this line
    //
    int oCurrentClient = getCurrentUserID(pSocket);
    QByteArray data = pSocket->readAll();
    qDebug() << "Data from client" << data;

    QStringList sValueList = QString::fromStdString(data.toStdString()).split(",");
    QString sRequestType = sValueList[0];
    sValueList.removeAll(sRequestType);
    qDebug() << sValueList;

    switch (sRequestTypeMap[sRequestType])
    {
    case 0:
        {
            if (1 != sValueList.size())
            {
                qDebug() << "ERROR  Invalid datatype on registration mode.";
                exit(1);
            }
            QStringList sRegisterValue = sValueList[0].split(" ");
            qDebug() << "Data for settings" << sRegisterValue;

            m_oGameSettins.m_nPlayerCount = sRegisterValue[0].toInt();
            m_oGameSettins.m_nRowCount = sRegisterValue[1].toInt();
            m_oGameSettins.m_nColCount = sRegisterValue[2].toInt();
            m_oGameSettins.m_nGridType = sRegisterValue[3].toInt();
            m_pLogic = new CLogic(m_oGameSettins.m_nColCount, m_oGameSettins.m_nRowCount, m_oGameSettins.m_nGridType);
            //
            /// Save initial gamer name
            //
            if (5 == sRegisterValue.size())
            {
                m_oScoreMap[sRegisterValue[4]] = 0;
                m_oClientNamesMap[oCurrentClient] = sRegisterValue[4];
            }
        }
        break;
    case 1:
        {
            /// Report settings for client view
            //
            QStringList sSetings;
            sSetings << "RR" << QString::number(2)
                     << QString::number(m_oGameSettins.m_nRowCount)
                     << QString::number(m_oGameSettins.m_nColCount)
                     << QString::number(m_oGameSettins.m_nGridType)
                     << QString::number(m_oGameSettins.m_nPlayerCount);
            sendMessage(sSetings.join(QString(" ")), oCurrentClient);
        }
        break;
    case 2:
        {
            /// Returns Game bord data
            //
            size_t count = 0;
            std::map<std::pair<size_t, size_t>, std::pair<int, short>> mapIndexes = m_pLogic->checkField(0, 0, count, true);
            qDebug() << "Read Game bord data";
            QString sValue;
            for (const auto& oValue : mapIndexes)
            {
                sValue.append(QString::number(oValue.first.first) + ",");
                sValue.append(QString::number(oValue.first.second) + ",");
                sValue.append(QString::number(oValue.second.first) + ",");
                sValue.append(QString::number(oValue.second.second) + ",");
            }
            sValue += m_oServerClients[oCurrentClient] ?  "YOUR TURN" : "WAIT FOR THE NEXT PLAYERS";
            sendMessage(sValue, oCurrentClient);
        }
        break;
    case 3:
        {
            if (2 != sValueList.size())
            {
                qDebug() << "ERROR  Invalid datatype on data  mode.";
                exit(1);
            }
            //
            /// Enable this one to connecte sync for players
            //
            if (!m_oServerClients[oCurrentClient]) return;

            //
            std::map<std::pair<size_t, size_t>, std::pair<int, short>> mapIndexes;
            if (m_pLogic != nullptr)
            {
                size_t count = 0;
                mapIndexes = m_pLogic->checkField(sValueList.first().toInt(), sValueList.last().toInt(), count);
                m_oScoreMap[m_oClientNamesMap[oCurrentClient]] += (count - m_nCount);
                m_nCount = count;
                if (count == m_oGameSettins.m_nColCount * m_oGameSettins.m_nRowCount)
                    m_bGameOver = true;
            }

            QString sValue;
            for (const auto& oValue : mapIndexes)
            {
                sValue.append(QString::number(oValue.first.first) + ",");
                sValue.append(QString::number(oValue.first.second) + ",");
                sValue.append(QString::number(oValue.second.first) + ",");
                sValue.append(QString::number(oValue.second.second) + ",");
            }

            //
            /// Deactivate current client
            /// Do not change this lines
            //
            m_oServerClients[oCurrentClient] = false;
            //
            /// Activate next pending client
            //
            m_oServerClients[getNextClientIndex(pSocket)] = true;
            //
            /// Notify all clients about updates
            //
            QStringList sValueList;
            QString  sGameValue = "";
            if (m_bGameOver)
            {
                foreach (int user, m_users.keys())
                {
                    sValueList << m_oClientNamesMap[user] << QString::number(m_oScoreMap[m_oClientNamesMap[user]]);
                }
                sGameValue = sValueList.join(".");
            }

            foreach (int user, m_users.keys())
            {
                QString sForUser = sValue;
                sForUser += m_oServerClients[user] ?  "YOUR TURN" : "WAIT FOR THE NEXT PLAYERS";
                if (m_bGameOver)
                    sForUser += "," + sGameValue;

                sendMessage(sForUser, user);
            }

            if (m_bGameOver)
            {
                cleanup();
            }
        }
        break;
    case 4:
        {
            if (1 != sValueList.size())
            {
                qDebug() << "ERROR  Invalid datatype on name registration mode.";
                exit(1);
            }
            m_oScoreMap[sValueList[0]] = 0;
            m_oClientNamesMap[oCurrentClient] = sValueList[0];
        }
        break;
    default:
        break;
    }
}

void CGameServer::onFlushTimerTick()
{
    std::flush(std::cout);
}

void CGameServer::onDisconnected()
{
    QTcpSocket* pSocket = (QTcpSocket *) sender();
    int removeUser = getCurrentUserID(pSocket);
    if (removeUser != -1)
    {
        m_oServerClients.remove(removeUser);
        m_users.remove(removeUser);
        qDebug() << "Client" << removeUser << "disconnected from game";
    }

    if (0 == m_users.size())
        cleanup();
}


int CGameServer::getNextClientIndex(QTcpSocket* pSocket)
{
    int oCurrentUser = getCurrentUserID(pSocket);
    QList<int> keys = m_users.keys();
    for(size_t i = 0; i < keys.size(); ++i)
    {
        if (keys[i] == oCurrentUser)
        {
            if (i == keys.size() - 1)
                return 0;
            else
                return i + 1;
        }
    }
}

int CGameServer::getCurrentUserID(QTcpSocket* pSocket)
{
    foreach (int user, m_users.keys())
    {
        if (pSocket == m_users[user]) return user;
    }
    return -1;
}

void CGameServer::cleanup()
{
    delete m_pLogic;
    m_pLogic = nullptr;

    m_nUserCount = 0;
    m_nCount = 0;
    m_oScoreMap.clear();
    m_oServerClients.clear();
    m_oClientNamesMap.clear();
    m_oGameSettins.m_nColCount = 0;
    m_oGameSettins.m_nRowCount = 0;
    m_oGameSettins.m_nGridType = 0;
    m_oGameSettins.m_nPlayerCount = 4;
    m_bGameOver = false;

    if (0 != m_users.size())
    {
        foreach(int user, m_users.keys())
        {
            m_users[user]->close();
            delete m_users[user];
            m_users[user] = nullptr;
        }
        m_users.clear();
    }
}

