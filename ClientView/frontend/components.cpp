#include "components.h"

#include <QBrush>
#include <QFont>

CButton::CButton(QString sName, QGraphicsItem *pParent)
    : QGraphicsRectItem(pParent)
{
    // draw rect
    setRect(0, 0, 200, 70);
    QBrush oBrush;
    oBrush.setStyle(Qt::SolidPattern);
    oBrush.setColor(Qt::darkCyan);
    setBrush(oBrush);

    // draw text
    m_pText = new QGraphicsTextItem(sName, this);

    int xPos = rect().width() / 2 - m_pText->boundingRect().width() / 2;
    int yPos = rect().height() / 2 - m_pText->boundingRect().height() / 2;

    m_pText->setFont(QFont("times", 16));
    m_pText->setPos(xPos, yPos);

    // allow responding to hover events
    setAcceptHoverEvents(true);
}

void CButton::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
    Q_UNUSED(pEvent);

    emit sigClicked();
}

void CButton::hoverEnterEvent(QGraphicsSceneHoverEvent* pEvent)
{
    Q_UNUSED(pEvent);
    // change color to cyan
    QBrush oBrush;
    oBrush.setStyle(Qt::SolidPattern);
    oBrush.setColor(Qt::cyan);
    setBrush(oBrush);
}

void CButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* pEvent)
{
    Q_UNUSED(pEvent);

    // change color to Darkcyan
    QBrush oBrush;
    oBrush.setStyle(Qt::SolidPattern);
    oBrush.setColor(Qt::darkCyan);
    setBrush(oBrush);
}
