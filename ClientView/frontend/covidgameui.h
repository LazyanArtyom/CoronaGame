#ifndef COVIDGAME_H
#define COVIDGAME_H

#include <QPointer>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>
#include <QMap>

#include "tcpclient.h"
#include "cell.h"

class CGameScene;

///////////////////////////////////////
/// \brief The CCovidGame class
//
class CCovidGame : public QGraphicsView
{
    Q_OBJECT
public:
    CCovidGame(QSize oScreenSize, QWidget* pParent = nullptr);
    void StartMenu();

protected:
    void keyPressEvent(QKeyEvent* pEvent) override;
    void resizeEvent(QResizeEvent* pEvent) override;

public slots:
    void connectToserver();
    void onRun();
    void onDecreaseScore();
    void onGameOver(QMap<QString, int>);
    void onItemSelected(QPair<int, int> parIndex);
    void onUpdateFileds(QList<SValue> lstValue);
    //
    /// Mezi qfur tvoxi .................
    //
    void onFirstClient();
    void onClientSettings(int nRow, int nCol, int nGridType, int nPlayerCount);

private:
    void onGameUpdateMsgLabel(QString msg);
    void onTcpClientReport(QString msg);
    void onTcpClientReceivedData(QByteArray data);

protected:
    void wheelEvent(QWheelEvent* pEvent);

private:
    CGameScene* m_pScene = nullptr;
    QSize       m_oScreenSize;
    CClient     m_oClient;
    CGameBoard* m_pBoard;
    bool        m_bFirstClick = true;
    bool        m_bFirstClent = true;
    size_t      m_nRowCount = 0;
    size_t      m_nColCount = 0;
    size_t      m_nGridType = 0;
    size_t      m_nPlayerCount = 0;
    QString     m_sPlayerName = "";

    QLineEdit*  m_pwPlayerName = nullptr;
    QSpinBox*   m_pwNumPlayers = nullptr;
    QSpinBox*   m_pwRowNumber  = nullptr;
    QSpinBox*   m_pwColNumber  = nullptr;
    QComboBox*  m_pwGridType   = nullptr;
};

#endif // !COVIDGAME_H
