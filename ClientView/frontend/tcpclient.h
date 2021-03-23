#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

///////////////////////////////////////////////////////////////////
/// \brief The SValue struct
//
struct SValue
{
    SValue(int nRow, int nCol, int nValue, short nPlvac)
        : nRow(nRow), nCol(nCol), nValue(nValue), nPlvac(nPlvac)
    {}

    int nRow;
    int nCol;
    int nValue;
    short nPlvac;
};

///////////////////////////////////////////////////////////////////
/// \brief The CClient class
//
class CClient : public QObject
{
    Q_OBJECT
public:
    CClient();

    bool isConnected();
    void connectToServer();
    void disconnectFromServer();
    void sendMessage(QString msg);

signals:
    void sigReport(QString);
    void sigUpdateFields(QList<SValue> lstValue);
    void sigMessageForPlayer(const QString&);
    void sigGameOver(QMap<QString, int>);
    void sigFirstClient();
    void sigClientSettings(int nRow, int nCol, int nGridType, int nPlayerCount);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onReadyRead();
    void onFlushTimerTick();

private:
    QTcpSocket* m_pSocket = nullptr;
    QTimer*     m_pFlushTimer = nullptr;
};

#endif // !CLIENT_H
