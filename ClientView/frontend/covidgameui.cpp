#include "covidgameui.h"

#include <QTimer>
#include <QGraphicsTextItem>
#include <QGraphicsWidget>
#include <QBoxLayout>
#include <QRadioButton>
#include <QGraphicsLinearLayout>
#include "cell.h"
#include "components.h"

#include <QDebug>
#include <chrono>
#include <thread>

CCovidGame::CCovidGame(QSize oScreenSize, QWidget *pParent)
    : QGraphicsView(pParent), m_oScreenSize(oScreenSize)
{
    QGraphicsScene* pScene = new QGraphicsScene(this);
    setScene(pScene);
    pScene->setSceneRect(0, 0, m_oScreenSize.width(), m_oScreenSize.height() - 25);

    //setBackgroundBrush(QBrush(QImage(":/graphical/covidbg.jpg")));
    setBackgroundBrush(QBrush(QImage(":/graphical/SpaceInvadersBg.jpg")));

//  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("border-style:none");
    setRenderHint(QPainter::Antialiasing);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    /// Request settings data from Server
    //
    connectToserver();
}

void CCovidGame::onRun()
{
    QStringList sSettings;
    sSettings << QString::number(m_pwNumPlayers->value());
    m_nRowCount = m_pwRowNumber->value();
    sSettings << QString::number(m_nRowCount);
    m_nColCount = m_pwColNumber->value();
    sSettings << QString::number(m_nColCount);
    sSettings << QString::number(m_pwGridType->currentIndex());
    QStringList sMessageLst = QStringList() << "RR";
    if (m_pwRowNumber->isEnabled())
    {
        sSettings << m_pwPlayerName->text();
        sMessageLst << sSettings.join(" ");
        m_oClient.sendMessage(sMessageLst.join(","));
    }
    else
    {
        QStringList sMessageLst = QStringList() << "NRR" << m_pwPlayerName->text();
        m_oClient.sendMessage(sMessageLst.join(","));
    }
    std::this_thread::sleep_for(std::chrono::microseconds(2000));
    /////////////////////////////////////////////
    m_oClient.sendMessage(QString("BGR, "));
    std::this_thread::sleep_for(std::chrono::microseconds(2000));

    scene()->clear();
    //setCursor(Qt::BlankCursor);

    m_pScene = new CGameScene();
    setScene(m_pScene);
    m_pScene->setSceneRect(0, 0, m_oScreenSize.width(), m_oScreenSize.height() - 25);

    connect(m_pScene, SIGNAL(sigItemSelected(QPair<int, int>)), this, SLOT(onItemSelected(QPair<int, int>)));
    connect(&m_oClient, SIGNAL(sigUpdateFields(QList<SValue>)), this, SLOT(onUpdateFileds(QList<SValue>)));
    connect(&m_oClient, SIGNAL(sigMessageForPlayer(const QString&)), m_pScene, SLOT(onSetMessage(const QString&)));
    connect(&m_oClient, SIGNAL(sigGameOver(QMap<QString,int>)), this, SLOT(onGameOver(QMap<QString,int>)));

    m_pBoard = new CGameBoard();
    //pWnd->setOpacity(0.9);
    m_pBoard->setPos(0, 0);
    m_pBoard->resize(width(), height());

    QVector<QVector<QVariant>> mtxBoard;
    for (int i = 0; i < m_nRowCount; ++i)
    {
        mtxBoard.push_back(QVector<QVariant>());
        for (int j = 0; j < m_nColCount; ++j)
        {
            mtxBoard[i].push_back(true);
        }
    }

    m_pBoard->SetBoard(mtxBoard);
    m_pScene->AddGameBoard(m_pBoard);
}

void CCovidGame::StartMenu()
{
    scene()->clear();
    QPointF oCenter = mapToScene(viewport()->rect().center());

    // Main Logo
    QPixmap pixmap(":/graphical/covidlogo.png");
    QGraphicsPixmapItem *mainLogo = new QGraphicsPixmapItem(pixmap);
    mainLogo->setPos(oCenter.x() - mainLogo->boundingRect().width() / 2 + 700, 0);
    scene()->addItem(mainLogo);

    // alien logo
    QPixmap alienLogo(":/graphical/invader.png");
    QPixmap newAlienLogo = alienLogo.scaled(QSize(800, 800), Qt::KeepAspectRatio);
    QGraphicsPixmapItem *alien = new QGraphicsPixmapItem(newAlienLogo);
    alien->setPos(oCenter.x() + alien->boundingRect().width() + 400, 270);
    scene()->addItem(alien);

    // create Play button
    CButton *playButton = new CButton(QString("Play"));
    int posXplayButton = oCenter.x() - playButton->boundingRect().width() / 2 + 700;
    int posYplayButton = 500;
    playButton->setPos(posXplayButton, posYplayButton);

    connect(playButton, SIGNAL(sigClicked()), this, SLOT(onRun()));
    scene()->addItem(playButton);

    // create quit button
    CButton *quitButton = new CButton(QString("Quit"));
    int posXquitButton = oCenter.x() - quitButton->boundingRect().width() / 2 + 700;
    int posYquitButton = 620;
    quitButton->setPos(posXquitButton, posYquitButton);

    connect(quitButton, SIGNAL(sigClicked()), this, SLOT(close()));
    scene()->addItem(quitButton);


    // To doooo if (isFirstPlayer)
    // ***************************
    QWidget* pwSettings = new QWidget();
    pwSettings->resize(200, scene()->height());

    QVBoxLayout* pMainLayout = new QVBoxLayout();
    //pMainLayout->setMargin(0);
    //pMainLayout->setSpacing(0);
    //pMainLayout->setStretch(5, 10);
    pMainLayout->setContentsMargins(5, 10, 5, 10);
    pMainLayout->setAlignment(Qt::AlignTop);

    m_pwPlayerName = new QLineEdit();
    m_pwPlayerName->setPlaceholderText("Enter Name:");
    pMainLayout->addWidget(m_pwPlayerName);

    QHBoxLayout* pHLayoutNumPlayers = new QHBoxLayout();
    QLabel* lblNumPlayers = new QLabel("Num Players:");
    m_pwNumPlayers = new QSpinBox();
    pHLayoutNumPlayers->addWidget(lblNumPlayers);
    pHLayoutNumPlayers->addWidget(m_pwNumPlayers);
    pMainLayout->addLayout(pHLayoutNumPlayers);

    QLabel* lblDimension = new QLabel("Dimension:");
    pMainLayout->addWidget(lblDimension);

    QHBoxLayout* pHLayoutRow = new QHBoxLayout();
    QLabel* lblRow = new QLabel("Row:");
    m_pwRowNumber = new QSpinBox();
    m_pwRowNumber->setRange(1, 100);
    pHLayoutRow->addWidget(lblRow);
    pHLayoutRow->addWidget(m_pwRowNumber);
    pMainLayout->addLayout(pHLayoutRow);

    QHBoxLayout* pHLayoutCol = new QHBoxLayout();
    QLabel* lblCol = new QLabel("Col:");
    m_pwColNumber = new QSpinBox();
    m_pwColNumber->setRange(1, 100);
    pHLayoutCol->addWidget(lblCol);
    pHLayoutCol->addWidget(m_pwColNumber);
    pMainLayout->addLayout(pHLayoutCol);

    QHBoxLayout* pHLayoutGridType = new QHBoxLayout();
    QLabel* lblGridType = new QLabel("Grid Type:");
    m_pwGridType =   new QComboBox();
    m_pwGridType->addItem(QString("THOR"));
    m_pwGridType->addItem(QString("CLOSED BORDER"));
    m_pwGridType->addItem(QString("OPEN BORDER"));
    pHLayoutGridType->addWidget(lblGridType);
    pHLayoutGridType->addWidget(m_pwGridType);
    pMainLayout->addLayout(pHLayoutGridType);

    pwSettings->setLayout(pMainLayout);
    scene()->addWidget(pwSettings);
}

void CCovidGame::resizeEvent(QResizeEvent* pEvent)
{
    QGraphicsView::resizeEvent(pEvent);
}

void CCovidGame::connectToserver()
{
    connect(&m_oClient, &CClient::sigFirstClient, this, &CCovidGame::onFirstClient);
    connect(&m_oClient, &CClient::sigClientSettings, this, &CCovidGame::onClientSettings);
    m_oClient.connectToServer();
    std::this_thread::sleep_for(std::chrono::microseconds(2000));
    //
    ///
    //
    m_oClient.sendMessage(QString("SGR, "));
    std::this_thread::sleep_for(std::chrono::microseconds(2000));

    StartMenu();
}

void CCovidGame::keyPressEvent(QKeyEvent *pEvent)
{
    Q_UNUSED(pEvent);
    QGraphicsView::keyPressEvent(pEvent);
}

void CCovidGame::onDecreaseScore()
{
  // TO DO
}

void CCovidGame::onGameOver(QMap<QString, int> oNameScoreMap)
{
    scene()->clear();
    QPointF oCenter = mapToScene(viewport()->rect().center());

    // Main Logo
    QPixmap pixmap(":/graphical/covidlogo.png");
    QGraphicsPixmapItem *mainLogo = new QGraphicsPixmapItem(pixmap);
    mainLogo->setPos(oCenter.x() - mainLogo->boundingRect().width() / 2, 0);
    scene()->addItem(mainLogo);

    // alien logo
    QPixmap alienLogo(":/graphical/invader.png");
    QPixmap newAlienLogo = alienLogo.scaled(QSize(800, 800), Qt::KeepAspectRatio);
    QGraphicsPixmapItem *alien = new QGraphicsPixmapItem(newAlienLogo);
    alien->setPos(oCenter.x() + alien->boundingRect().width() - 300, 270);
    scene()->addItem(alien);

    // create quit button
    CButton *quitButton = new CButton(QString("Quit"));
    int posXquitButton = oCenter.x() - quitButton->boundingRect().width() / 2;
    int posYquitButton = 620;
    quitButton->setPos(posXquitButton, posYquitButton);

    connect(quitButton, SIGNAL(sigClicked()), this, SLOT(close()));
    scene()->addItem(quitButton);

    QStringList sScores;
    for (QMap<QString, int>::iterator obj = oNameScoreMap.begin(); obj != oNameScoreMap.end(); ++obj)
    {
        sScores << QString(obj.key() + " : " + QString::number(obj.value()));
    }

    // type Score
    QGraphicsTextItem *text = new QGraphicsTextItem(sScores.join("\n"));
    text->setFont(QFont("times", 32));
    text->setDefaultTextColor(Qt::red);
    text->setPos(this->width() / 2 - text->boundingRect().width() / 2 - 20, this->height() / 2 + 300);
    scene()->addItem(text);

    // type Score
    QGraphicsTextItem *textGame = new QGraphicsTextItem("GAME OVER");
    textGame->setFont(QFont("times", 72));
    textGame->setDefaultTextColor(Qt::red);
    textGame->setPos(quitButton->pos().x() - textGame->boundingRect().width() / 2 + 100, quitButton->pos().y() + 100);
    scene()->addItem(textGame);
}

void CCovidGame::onFirstClient()
{
    m_bFirstClent = false;
}

void CCovidGame::onClientSettings(int nRow, int nCol, int nGridType, int nPlayerCount)
{
    m_nRowCount = nRow;
    m_nColCount = nCol;
    m_nGridType = nGridType;
    m_nPlayerCount = nPlayerCount;
    m_pwColNumber->setValue(nCol);
    m_pwRowNumber->setValue(nRow);
    m_pwGridType->setCurrentIndex(nGridType);
    m_pwNumPlayers->setValue(nPlayerCount);
    if (0 != nRow && 0 != nCol)
    {
        m_pwColNumber->setEnabled(false);
        m_pwRowNumber->setEnabled(false);
        m_pwGridType->setEnabled(false);
        m_pwNumPlayers->setEnabled(false);
    }
}

void CCovidGame::onItemSelected(QPair<int, int> parIndex)
{
    if (m_bFirstClick)
    {
        m_bFirstClick = false;
        return;
    }
    qDebug() << "Goodddd : " << parIndex.first << " : " << parIndex.second;
    QStringList sMessage;
    sMessage << "DGR" << QString::number(parIndex.first) << QString::number(parIndex.second);
    m_oClient.sendMessage(sMessage.join(","));
}

void CCovidGame::onUpdateFileds(QList<SValue> lstValue)
{
    for (const auto& oValue : lstValue)
    {
        m_pScene->SetItemValue(QPair<int, int>(oValue.nRow, oValue.nCol), oValue.nValue, oValue.nPlvac);
    }
}

void CCovidGame::wheelEvent(QWheelEvent* pEvent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double oScaleFactor = 1.15;
    if (pEvent->delta() > 0)
        scale(oScaleFactor, oScaleFactor);
    else
        scale(1 / oScaleFactor, 1 / oScaleFactor);
}
