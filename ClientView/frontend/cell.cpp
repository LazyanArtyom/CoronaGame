#include "cell.h"

#include <QLabel>
#include <QGradient>
#include <QPainter>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include <QDebug>

CLayoutItem::CLayoutItem(QGraphicsItem *pParent)
    : QGraphicsLayoutItem(), QGraphicsItem(pParent)
{
    setGraphicsItem(this);
    //m_oPixmap = m_oPixmap.scaled(50, 50);
}

void CLayoutItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* oOpt,
                       QWidget* pWidget)
{
    Q_UNUSED(pWidget);
    Q_UNUSED(oOpt);

    QRectF oRect(QPointF(0, 0), geometry().size());
    //const QSize pmSize = m_oPixmap.size();
    const QSize pmSize = m_oSize;

    // paint a background rect (with gradient)
    QGradientStops oStops;
    QLinearGradient oGradient(oRect.topLeft(), oRect.topLeft() + QPointF(200,200));
    oStops << QGradientStop(0.0, QColor(60, 60,  60));
    oStops << QGradientStop(oRect.height() / 2 / oRect.height(), m_oColor);

    oStops << QGradientStop(1.0, QColor(215, 215, 215));
    oGradient.setStops(oStops);
   // pPainter->setBrush(QBrush(oGradient));
    pPainter->setBrush(QBrush(m_oColor));
    pPainter->drawRect(oRect);

    // paint a rect around the pixmap (with gradient)
    QPointF oPixPos = oRect.center() - (QPointF(pmSize.width(), pmSize.height()) / 2);
    pPainter->drawPixmap(oPixPos, m_oPixmap);
    QTextOption oOption;
    oOption.setAlignment(Qt::AlignCenter);
    pPainter->drawText(oRect, QString::number(m_nValue), oOption);
    QFont oFont;
    oFont.setPixelSize(10);
    pPainter->setFont(oFont);

    // QFontMetrics oFontmetrix(pPainter->font());

}

void CLayoutItem::setSize(const QSize &oSize)
{
    m_oSize = oSize;
}

QRectF CLayoutItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), geometry().size());
}

void CLayoutItem::setGeometry(const QRectF& oGeometry)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(oGeometry);
    setPos(oGeometry.topLeft());
}

QSizeF CLayoutItem::sizeHint(Qt::SizeHint nSizeHint, const QSizeF &oConstraint) const
{
    switch (nSizeHint)
    {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        // Do not allow a size smaller than the pixmap with two frames around it.
        //return m_oPixmap.size() + QSize(12, 12);
        return m_oSize;
    case Qt::MaximumSize:
        return QSizeF(1000, 1000);
    default:
        break;
    }

    return oConstraint;
}

void CLayoutItem::setPixmap(const QPixmap& oPixmap)
{
    m_oPixmap = oPixmap;
    update();
}

void CLayoutItem::setColor(const QColor& oColor)
{
    m_oColor = oColor;
    update();
}

void CLayoutItem::setValue(int nValue)
{
    m_nValue = nValue;
    update();
}

void CLayoutItem::setIndex(const QPair<int, int> parIndex)
{
    m_parIndex = parIndex;
}

QPair<int, int> CLayoutItem::getIndex() const
{
    return m_parIndex;
}

/////////////////////////////////////////////
CGameBoard::CGameBoard(QGraphicsWidget *pParent) : QGraphicsWidget(pParent, Qt::Window)
{
    setWindowTitle(tr("Basic Graphics Layout"));
   // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CGameBoard::SetBoard(const QVector<QVector<QVariant> > &mtxBoard)
{
    QGraphicsGridLayout *pGrid = new QGraphicsGridLayout();
    pGrid->setSpacing(0);
   // pGrid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(pGrid);

    //pGrid->setPreferredSize(40, 40);

    m_mtxBoard = mtxBoard;
    const int nDimension = mtxBoard.size();

    CLayoutItem* pItem = nullptr;
    for (int nRow = 0; nRow < mtxBoard.size(); ++nRow)
    {
        for (int nCol = 0; nCol < mtxBoard[nRow].size(); ++nCol)
        {
            pItem = new CLayoutItem;
            pItem->setColor(Qt::green);
            //pItem->setSize(QSize(size().width() / nDimension, size().height() / nDimension));
            pItem->setIndex(QPair<int, int>(nRow, nCol));
          //  pItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            //pItem->setPixmap(QPixmap(QLatin1String(":/graphical/covid.png")));
            pGrid->addItem(pItem, nRow, nCol);
        }
    }
}

void CGameBoard::SetValue(const int nRow, const int nCol, QVariant oValue, int nValue)
{
    QGraphicsGridLayout *pGrid = static_cast<QGraphicsGridLayout*>(layout());
    Q_ASSERT(pGrid != nullptr);

    ILayoutItem* pItem = dynamic_cast<ILayoutItem*>(pGrid->itemAt(nRow, nCol));
    Q_ASSERT(pItem != nullptr);

    switch (nValue)
    {
    case 0:
        pItem->setColor(Qt::green);
        break;
    default:
        pItem->setColor(Qt::red);
        break;
    }

    pItem->setValue(oValue.toInt());
    qDebug() << "SetValue" << oValue.toBool();
}

void CGameBoard::Clear()
{

}

CGameScene::CGameScene(QObject *pObj)
    : QGraphicsScene(pObj)
{
    m_pMessageView = addWidget(new QLabel("YOUR TURN"));
    m_pMessageView->setPos(0, 0);
    m_pMessageView->setZValue(1);
    m_pMessageView->resize(width(), height() / 15);
    addItem(m_pMessageView);
}

void CGameScene::SetItemValue(const QPair<int, int> parIndex, QVariant oValue, int nValue)
{
    if (m_pGameBoard != nullptr)
        m_pGameBoard->SetValue(parIndex.first, parIndex.second, oValue, nValue);
}

void CGameScene::onSetMessage(const QString& sMessage)
{
    if (m_pMessageView == nullptr)
        return;

    static_cast<QLabel*>(m_pMessageView->widget())->setText(sMessage);
}

void CGameScene::AddGameBoard(CGameBoard *pGameBoard)
{
    m_pGameBoard = pGameBoard;
    addItem(pGameBoard);
}

void CGameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        QGraphicsItem* pItem = itemAt(pEvent->scenePos(), QTransform());
        if (pItem != nullptr)
        {
            ILayoutItem* pLayoutItem = dynamic_cast<ILayoutItem*>(pItem);
            if (pLayoutItem != nullptr)
            {
                QPair<int, int> parIndex = pLayoutItem->getIndex();
                emit sigItemSelected(parIndex);
            }
        }
    }

    QGraphicsScene::mouseReleaseEvent(pEvent);
}
