#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class CLogic;

struct SSetings
{
    int m_nPlayerCount = 4;
    int m_nRowCount = -1;
    int m_nColCount = -1;
    int m_nGridType = -1;
};

/////////////////////////////////////////////////////////////////////
/// \brief The CGameServer class
//
class CGameServer : public QTcpServer
{
    Q_OBJECT
public:
    //
    /// \brief CGameServer constructor
    /// \param parent
    //
    CGameServer(QObject* parent = nullptr);
    //
    /// \brief sendMessage, Sending Message to client
    /// \param msg
    /// \param user
    //
    void sendMessage(QString msg, int user);

private slots:
    //
    /// \brief onNewConnection, New pending connection handler
    //
    void onNewConnection();
    //
    /// \brief onReadyRead, Client data handler
    //
    void onReadyRead();
    //
    /// \brief onFlushTimerTick, Flush client data stream to std::out
    //
    void onFlushTimerTick();
    //
    /// \brief onDisconnected, Client disconnection handler
    //
    void onDisconnected();

private:
    int getNextClientIndex(QTcpSocket* pSocket);
    //
    ///
    //
    int getCurrentUserID(QTcpSocket* pSocket);
    //
    ///
    //
    void cleanup();

private:
    int                       m_nUserCount = 0;
    size_t                    m_nCount = 0;
    bool                      m_bSettingsInicialized = false;
    bool                      m_bGameOver = false;
    QList<QString>            m_socketBuffers;
    QMap<int, bool>           m_oServerClients;
    QMap<int, QTcpSocket*>    m_users;
    QTimer*                   m_pFlushTimer = nullptr;
    CLogic*                   m_pLogic = nullptr;
    SSetings                  m_oGameSettins;
    QMap<QString, int>        m_oScoreMap;
    QMap<int, QString>        m_oClientNamesMap;
};

#endif // !GAMESERVER_H
