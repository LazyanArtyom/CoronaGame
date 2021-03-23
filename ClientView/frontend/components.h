#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QObject>
#include <QString>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class CButton : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    CButton(QString sName, QGraphicsItem *pParent = nullptr);

    // public metods (events) for QGraphicsItem
    void mousePressEvent(QGraphicsSceneMouseEvent *pEvent);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent);

signals:
    void sigClicked();

private:
    QGraphicsTextItem* m_pText;

};

#endif // COMPONENTS_H
