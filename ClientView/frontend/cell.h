#ifndef CELL_H
#define CELL_H

#include <QPixmap>
#include <QGraphicsItem>

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsLayoutItem>
#include <QGraphicsLayout>
#include <QGraphicsSceneMouseEvent>

class ILayoutItem
{
public:
    virtual void setSize(const QSize& oSize) = 0;
    virtual void setPixmap(const QPixmap& oPixmap) = 0;
    virtual void setColor(const QColor& oColor) = 0;
    virtual void setIndex(const QPair<int, int> parIndex) = 0;
    virtual void setValue(int) = 0;

    virtual QPair<int, int> getIndex() const = 0;
    virtual ~ILayoutItem() {}
};

class CLayoutItem : public QGraphicsLayoutItem, public QGraphicsItem, public ILayoutItem
{
public:
    CLayoutItem(QGraphicsItem *pParent = nullptr);

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &oGeometry) override;
    QSizeF sizeHint(Qt::SizeHint nSizeHint,
                    const QSizeF &oConstraint = QSizeF()) const override;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *pPainter, const QStyleOptionGraphicsItem *oOpt,
               QWidget *pWidget = nullptr) override;

    // Inherited from ILayoutItem
    void setSize(const QSize& oSize) override;
    void setPixmap(const QPixmap& oPixmap) override;
    void setColor(const QColor& oColor) override;
    void setIndex(const QPair<int, int> parIndex) override;
    void setValue(int) override;

    QPair<int, int> getIndex() const override;

private:
    int     m_nValue;
    int m_nPlvac;
    QSize   m_oSize;
    QColor  m_oColor;
    QPixmap m_oPixmap;
    QPair<int, int>  m_parIndex;
};

///////////////////////////////////////
class CGameBoard : public QGraphicsWidget
{
    Q_OBJECT
public:
    CGameBoard(QGraphicsWidget* pParent = nullptr);

    void SetBoard(const QVector<QVector<QVariant>>& mtxBoard);
    void SetValue(const int nRow, const int nCol, QVariant bValue, int nValue);

    void Clear();

private:
    QVector<QVector<QVariant>> m_mtxBoard;
};

///////////////////////////////////////
class CGameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CGameScene(QObject* pObj = nullptr);

    void AddGameBoard(CGameBoard* pGameBoard);
    void SetItemValue(const QPair<int, int> parIndex, QVariant oValue, int nValue);

public slots:
    void onSetMessage(const QString& sMessage);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) override;

signals:
    void sigItemSelected(QPair<int, int> parIndex);

private:
    CGameBoard*             m_pGameBoard = nullptr;
    QGraphicsProxyWidget*   m_pMessageView = nullptr;
};

#endif // CELL_H
