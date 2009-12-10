#ifndef MEDDATABASENAVIGATORSELECTOR_H
#define MEDDATABASENAVIGATORSELECTOR_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorSelectorPrivate;

class medDatabaseNavigatorSelector : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)
    Q_PROPERTY(QString text READ text WRITE setText)

    Q_INTERFACES(QGraphicsItem)

public:
     medDatabaseNavigatorSelector(QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorSelector(void);

    QRectF rect(void) const;
    QRectF boundingRect(void) const;
    QString text(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setRect(const QRectF& rect);
    void setText(const QString& text);

private:
    medDatabaseNavigatorSelectorPrivate *d;
};

#endif // MEDDATABASENAVIGATORSELECTOR_H
